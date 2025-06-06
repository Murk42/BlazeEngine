#include "pch.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/TextSelection/TextSelectionRenderUnit.h"
#include "BlazeEngineCore/DataStructures/Rect.h"
#include "BlazeEngineCore/Debug/Logger.h"

namespace Blaze::UI
{
	static float Clamp(float value, float min, float max)
	{
		if (value < min)
			return min;
		if (value > max)
			return max;
		return value;
	}
	static inline Rectf InscribeRect(const Rectf& smaller, const Rectf& bigger)
	{
		Vec2f smallerPos1 = smaller.pos;
		Vec2f smallerPos2 = smaller.pos + smaller.size;
		Vec2f biggerPos2 = bigger.pos + bigger.size;
		smallerPos1.x = Clamp(smallerPos1.x, bigger.x, biggerPos2.x);
		smallerPos2.x = Clamp(smallerPos2.x, bigger.x, biggerPos2.x);
		smallerPos1.y = Clamp(smallerPos1.y, bigger.y, biggerPos2.y);
		smallerPos2.y = Clamp(smallerPos2.y, bigger.y, biggerPos2.y);
		return Rectf(smallerPos1, smallerPos2 - smallerPos1);
	}
	static bool RectIntersect(Rectf r1, Rectf r2)
	{
		return r1.x + r1.w > r2.x && r1.x < r2.x + r2.w &&
			r1.y + r1.h > r2.y && r1.y < r2.y + r2.h;
	}
	//Returns the rect of the culling node in transformSpace of the given transform
	static Rectf GetCullingRect(const NodeFinalTransform& transform, Vec2f position, float scale, float rotation, Vec2f right, Vec2f up)
	{		
		if (transform.rotation != rotation)
			return Rectf();
				
		Vec2f cullingSpacePos = transform.position - position;

		Rectf out;
		out.pos.x = right.DotProduct(cullingSpacePos) / scale;
		out.pos.y = up.DotProduct(cullingSpacePos) / scale;
		out.size = transform.size / scale;
		return out;		
	}
	
	TextSelectionRenderUnit::TextSelectionRenderUnit(TextSelection& selection, TextRenderUnitBase& textRenderUnit)
		: TextSelectionRenderUnitBase(selection, "TexturedRectRenderer"), textRenderUnit(textRenderUnit), 
		renderDataDirty(false), lineIndex(0), renderData()
	{
		renderData.blend = 1;
		renderData.color = 0xffffffff;
		renderData.alpha = 0.5f;
		renderData.texture = nullptr;
		renderData.uv1 = Vec2f(0, 0);
		renderData.uv2 = Vec2f(0, 0);
						
		selection.selectionChangedEventDispatcher.AddHandler<&TextSelectionRenderUnit::SelectionChangedEvent>(*this);
		textRenderUnit.renderDataUpdatedEventDispatcher.AddHandler<&TextSelectionRenderUnit::RenderDataUpdatedEvent>(*this);
	}
	TextSelectionRenderUnit::~TextSelectionRenderUnit()
	{	
		selection.selectionChangedEventDispatcher.RemoveHandler<&TextSelectionRenderUnit::SelectionChangedEvent>(*this);
		textRenderUnit.renderDataUpdatedEventDispatcher.RemoveHandler<&TextSelectionRenderUnit::RenderDataUpdatedEvent>(*this);
	}
	void TextSelectionRenderUnit::BeginStream()
	{		
		CleanRenderData();
		lineIndex = 0;		
	}
	void* TextSelectionRenderUnit::Advance()
	{
		if (lineIndex == lineSelections.Count())
			return nullptr;
		
		auto& lineSelection = lineSelections[lineIndex++];		
		renderData.pos = lineSelection.pos;
		renderData.right = lineSelection.right;
		renderData.up = lineSelection.up;

		return &renderData;
	}
	bool TextSelectionRenderUnit::CleanRenderData()
	{		
		textRenderUnit.CleanData();

		if (!renderDataDirty)
			return false;

		renderDataDirty = false;

		lineSelections.Clear();

		if (selection.GetHead() == selection.GetTail())
			return true;

		if (textRenderUnit.textContainer.CharacterCount() != selection.textContainer.CharacterCount())
		{
			BLAZE_ENGINE_GRAPHICS_ERROR("Preparing a text selection render unit for rendering with a text render unit and a text selection that have text containers with different character counts");			
			return true;
		}

		auto& characterData = textRenderUnit.GetCharacterData();

		if (characterData.Empty())
			return true;

		auto finalTransform = textRenderUnit.GetFinalTransform();
		float cos = Math::Cos(finalTransform.rotation);
		float sin = Math::Sin(finalTransform.rotation);
		Vec2f right = Vec2f(cos, sin);
		Vec2f up = Vec2f(-sin, cos);
		auto& linesData = textRenderUnit.GetLineData();

		const float selectionVerticalOffset = -(textRenderUnit.GetFontStyle()->fontPixelHeight* 0.05f);
		const float selectionHeight = textRenderUnit.GetFontStyle()->fontPixelHeight * 0.9f;

		//Rect of the culling node. The size will be (0, 0) if the culling node is nullptr or the rotations arent the same
		const Rectf cullingRect = GetCullingRect(textRenderUnit.GetCullingTransform(), finalTransform.position, finalTransform.scale, finalTransform.rotation, right, up);

		const uintMem selectionBegin = selection.GetBeginning();
		const uintMem selectionEnd = selection.GetEnd();

		const uintMem lineBegin = characterData[selectionBegin].lineIndex;
		const uintMem lineEnd = characterData[selectionEnd - 1].lineIndex + 1;

		for (uintMem lineIndex = lineBegin; lineIndex != lineEnd; ++lineIndex)
		{
			auto& line = linesData[lineIndex];

			uintMem characterEnd = std::min((uintMem)std::max((intMem)(selectionEnd - line.firstCharacterIndex), (intMem)0), line.characterCount);
			uintMem characterBegin = std::min((uintMem)std::max((intMem)(selectionBegin - line.firstCharacterIndex), (intMem)0), characterEnd);

			if (characterBegin == characterEnd)
				continue;

			//Rect of the selection
			Rectf rect{ 0, selectionVerticalOffset + line.pos.y, 0, selectionHeight };

			if (characterBegin == 0)
			{
				//The selection begins ahead of the characters in the line
				rect.x = characterData[characterBegin + line.firstCharacterIndex].pos.x;
			}
			else
			{
				//The selection begins between the characters in the line
				rect.x = (characterData[characterBegin + line.firstCharacterIndex].pos.x + characterData[characterBegin + line.firstCharacterIndex - 1].pos.x + characterData[characterBegin + line.firstCharacterIndex - 1].size.x) / 2;
			}
			
			//The selection ends behind the characters in the line
			rect.w = characterData[characterEnd + line.firstCharacterIndex - 1].pos.x + characterData[characterEnd + line.firstCharacterIndex - 1].size.x - rect.x;			

			if (cullingRect.size != Vec2f())
			{
				rect = InscribeRect(rect, cullingRect);

				if (rect.size == Vec2f())
				{
					//The rect is outsite the culling rect and thus should be culled completely
					continue;
				}
			}

			auto& lineSelection = *lineSelections.AddBack();

			//Calculate the rect of the selection in final transform space
			lineSelection.pos = finalTransform.position + (right * rect.pos.x + up * rect.pos.y) * finalTransform.scale;
			lineSelection.right = right * rect.size.x * finalTransform.scale;
			lineSelection.up = up * rect.size.y * finalTransform.scale;
		}

		return true;
	}	
	void TextSelectionRenderUnit::SelectionChangedEvent(const TextSelection::SelectionChangedEvent& event)
	{
		renderDataDirty = true;
	}
	void TextSelectionRenderUnit::RenderDataUpdatedEvent(const TextRenderUnitBase::RenderDataUpdatedEvent& event)
	{
		renderDataDirty = true;
	}

}