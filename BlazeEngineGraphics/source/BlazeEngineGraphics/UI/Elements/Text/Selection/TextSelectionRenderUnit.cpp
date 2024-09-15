#include "pch.h"
#include "BlazeEngineGraphics/UI/Elements/Text/Selection/TextSelectionRenderUnit.h"

namespace Blaze::UIGraphics
{
	static inline Rectf InscribeRect(const Rectf& smaller, const Rectf& bigger)
	{
		Vec2f smallerPos1 = smaller.pos;
		Vec2f smallerPos2 = smaller.pos + smaller.size;
		Vec2f biggerPos2 = bigger.pos + bigger.size;
		smallerPos1.x = std::clamp(smallerPos1.x, bigger.x, biggerPos2.x);
		smallerPos2.x = std::clamp(smallerPos2.x, bigger.x, biggerPos2.x);
		smallerPos1.y = std::clamp(smallerPos1.y, bigger.y, biggerPos2.y);
		smallerPos2.y = std::clamp(smallerPos2.y, bigger.y, biggerPos2.y);
		return Rectf(smallerPos1, smallerPos2 - smallerPos1);
	}
	static bool RectIntersect(Rectf r1, Rectf r2)
	{
		return r1.x + r1.w > r2.x && r1.x < r2.x + r2.w &&
			r1.y + r1.h > r2.y && r1.y < r2.y + r2.h;
	}
	//Returns the rect of the culling node in transformSpace of the given transform
	static Rectf GetCullingRect(const UI::NodeFinalTransform& transform, Vec2f position, float scale, float rotation, Vec2f right, Vec2f up)
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
	
	TextSelectionRenderUnit::TextSelectionRenderUnit(UIGraphics::TextRenderUnitBase& textRenderUnit)
		: TextSelectionRenderUnitBase("TexturedRectRenderer"), textRenderUnit(textRenderUnit), selectionBegin(0), selectionEnd(0),
		renderDataDirty(false), lineIndex(0), renderData()
	{
		renderData.blend = 1;
		renderData.color = 0xffffffff;
		renderData.alpha = 0.5f;
		renderData.texture = nullptr;
		renderData.uv1 = Vec2f(0, 0);
		renderData.uv2 = Vec2f(0, 0);

		textRenderUnit.renderDataUpdatedEventDispatcher.AddHandler(*this);
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
	bool TextSelectionRenderUnit::CleanData()
	{
		if (!textRenderUnit.CleanData())
			return false;		

		return true;
	}
	bool TextSelectionRenderUnit::CleanRenderData()
	{
		CleanData();

		if (!renderDataDirty)
			return false;

		renderDataDirty = false;

		lineSelections.Clear();

		uintMem characterCount = textRenderUnit.GetCharacterData().Count();

		if (selectionEnd == 0 || selectionBegin >= selectionEnd || selectionBegin >= characterCount)
			return true;		

		auto& characterData = textRenderUnit.GetCharacterData();

		if (characterData.Empty())
			return true;

		auto finalTransform = textRenderUnit.GetFinalTransform();
		float cos = Math::Cos(finalTransform.rotation);
		float sin = Math::Sin(finalTransform.rotation);
		Vec2f right = Vec2f(cos, sin);
		Vec2f up = Vec2f(-sin, cos);
		auto& linesData = textRenderUnit.GetLineData();

		const float selectionVerticalOffset = -(textRenderUnit.GetPixelFontHeight() * 0.05f);
		const float selectionHeight = textRenderUnit.GetPixelFontHeight() * 0.9f;

		//Rect of the culling node. The size will be (0, 0) if the culling node is nullptr or the rotations arent the same
		const Rectf cullingRect = GetCullingRect(textRenderUnit.GetCullingTransform(), finalTransform.position, finalTransform.scale, finalTransform.rotation, right, up);

		uintMem lineBegin = characterData[selectionBegin].lineIndex;
		uintMem lineEnd = characterData[selectionEnd - 1].lineIndex + 1;
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
	void TextSelectionRenderUnit::SetSelection(uintMem begin, uintMem end)
	{
		end = std::min(end, textRenderUnit.GetCharacterData().Count() - 1);
		begin = std::min(begin, end);				

		if (begin == end)
			begin = end = 0;
		
		selectionBegin = begin;
		selectionEnd = end;
		renderDataDirty = true;		
	}
	void TextSelectionRenderUnit::SetSelectionBegin(uintMem begin)
	{
		begin = std::min(begin, selectionEnd);		

		if (begin == selectionEnd)				
			begin = selectionEnd = 0;		
		
		selectionBegin = begin;
		renderDataDirty = true;		
	}
	void TextSelectionRenderUnit::SetSelectionEnd(uintMem end)
	{
		end = std::min(end, textRenderUnit.GetCharacterData().Count() - 1);		

		if (end == selectionBegin)
			end = selectionBegin = 0;

		selectionEnd = end;
		renderDataDirty = true;
	}	
	void TextSelectionRenderUnit::ClearSelection()
	{	
		if (selectionBegin != selectionEnd)
			renderDataDirty = true;				

		selectionBegin = 0;
		selectionEnd = 0;
	}
	uintMem TextSelectionRenderUnit::GetSelectionBegin()
	{
		CleanData();
		return selectionBegin; 
	}
	uintMem TextSelectionRenderUnit::GetSelectionEnd()
	{
		CleanData();
		return selectionEnd; 
	}
	bool TextSelectionRenderUnit::IsSelectionEmpty() 
	{
		CleanData();
		return selectionBegin == selectionEnd; 
	}	
	void TextSelectionRenderUnit::OnEvent(UIGraphics::TextRenderUnitBase::RenderDataUpdatedEvent)
	{
		renderDataDirty = true;
	}

}