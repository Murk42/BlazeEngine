#include "pch.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/TextCursor/TextCursorRenderUnit.h"

namespace Blaze::UI
{
	static inline float Clamp(float value, float min, float max)
	{
		if (value > max)
			return max;
		if (value < min)
			return min;
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
	//Returns the rect of the culling node in transformSpace of the given transform. The size if 0 if the rotations dont match or the cullingNode is nullptr
	static Rectf GetCullingRect(const NodeFinalTransform& transfrom, Vec2f position, float scale, float rotation, Vec2f right, Vec2f up)
	{		
		if (transfrom.rotation != rotation)
			return Rectf();
		
		Vec2f cullingSpacePos = transfrom.position - position;

		Rectf out;
		out.pos.x = right.DotProduct(cullingSpacePos) / scale;
		out.pos.y = up.DotProduct(cullingSpacePos) / scale;
		out.size = transfrom.size / scale;
		return out;
	}
	
	TextCursorRenderUnit::TextCursorRenderUnit(TextRenderUnitBase& textRenderUnit) :
		TextCursorRenderUnitBase("TexturedRectRenderer"), textRenderUnit(textRenderUnit), cursorCharacterIndex(0), renderDataDirty(false), rendered(true), shown(false)
	{		
		textRenderUnit.renderDataUpdatedEventDispatcher.AddHandler(*this);

		renderData.blend = 1;
		renderData.color = 0xffffffff;
		renderData.alpha = 1.0f;
		renderData.texture = nullptr;
		renderData.uv1 = Vec2f(0, 0);
		renderData.uv2 = Vec2f(0, 0);
	}
	TextCursorRenderUnit::~TextCursorRenderUnit()
	{
		textRenderUnit.renderDataUpdatedEventDispatcher.RemoveHandler(*this);
	}
	void TextCursorRenderUnit::BeginStream()
	{		
		CleanRenderData();
		rendered = false;
	}
	void* TextCursorRenderUnit::Advance()
	{
		if (rendered || culled || !shown || std::fmod((TimePoint::GetCurrentWorldTime() - lastTimeCursorSet).ToSeconds(), 1.0f) > 0.5f)
			return nullptr;

		rendered = true;

		return &renderData;
	}
	bool TextCursorRenderUnit::CleanData()
	{
		if (!textRenderUnit.CleanData())
			return false;

		ValidateCursorPos();

		return true;
	}
	bool TextCursorRenderUnit::CleanRenderData()
	{
		CleanData();

		if (!renderDataDirty)
			return false;

		renderDataDirty = false;
	
		auto& characterData = textRenderUnit.GetCharacterData();		
		
		if (cursorCharacterIndex >= characterData.Count() && !characterData.Empty())
		{
			culled = true;
			return true;
		}

		auto& linesData = textRenderUnit.GetLineData();
		auto& line = linesData[characterData[cursorCharacterIndex].lineIndex];		
		uintMem cursorLineCharacterIndex = cursorCharacterIndex - line.firstCharacterIndex;

		Vec2f cursorSize = Vec2f(0.05f, 0.8f) * textRenderUnit.GetPixelFontHeight();
		cursorSize = Vec2f(std::max(std::round(cursorSize.x), 1.0f), std::max(std::round(cursorSize.y), 1.0f));
		Rectf rect{ Vec2f(0, line.pos.y), cursorSize };

		if (line.characterCount == 0)
		{
			//The cursor is on an empty line
			rect.pos.x = line.pos.x - cursorSize.x / 2;
		}
		else if (cursorLineCharacterIndex == 0)
		{
			//The cursor is ahead of all characters in the line
			rect.pos.x = characterData[cursorCharacterIndex].pos.x - cursorSize.x;
		}
		else
		{
			//The cursor is behind all characters in the line
			rect.pos.x = characterData[cursorCharacterIndex - 1].pos.x + characterData[cursorCharacterIndex - 1].size.x;
		}		

		auto textFinalTransform = textRenderUnit.GetFinalTransform();
		float cos = Math::Cos(textFinalTransform.rotation);
		float sin = Math::Sin(textFinalTransform.rotation);
		Vec2f right = Vec2f(cos, sin);
		Vec2f up = Vec2f(-sin, cos);

		//Rect of the culling node. The size will be (0, 0) if the culling node is nullptr or the rotations arent the same
		Rectf cullingRect = GetCullingRect(textRenderUnit.GetCullingTransform(), textFinalTransform.position, textFinalTransform.scale, textFinalTransform.rotation, right, up);

		if (cullingRect.size != Vec2f())
		{
			rect = InscribeRect(rect, cullingRect);

			if (rect.size == Vec2f())
			{
				//The rect is outsite the culling rect and thus should be culled completely
				culled = true;
				return true;
			}
		}

		renderData.pos = textFinalTransform.position + (right * rect.pos.x + up * rect.pos.y) * textFinalTransform.scale;
		renderData.right = right * cursorSize.x * textFinalTransform.scale;
		renderData.up = up * cursorSize.y * textFinalTransform.scale;
		culled = false;

		return true;
	}	
	void TextCursorRenderUnit::SetCursorPosBeforeCharacter(uintMem characterIndex)
	{
		auto& characterData = textRenderUnit.GetCharacterData();

		if (characterIndex != 0 && characterIndex >= characterData.Count())
		{
			SetCursorPosToEnd();
			return;
		}

		cursorCharacterIndex = characterIndex;
		renderDataDirty = true;
		lastTimeCursorSet = TimePoint::GetCurrentWorldTime();
	}
	void TextCursorRenderUnit::SetCursorPosAfterCharacter(uintMem characterIndex)
	{		
		auto& characterData = textRenderUnit.GetCharacterData();

		++characterIndex;
		if (characterIndex >= characterData.Count())
		{
			SetCursorPosToEnd();
			return;
		}

		cursorCharacterIndex = characterIndex;
		renderDataDirty = true;
		lastTimeCursorSet = TimePoint::GetCurrentWorldTime();
	}
	void TextCursorRenderUnit::SetCursorPosToBeginning()
	{
		SetCursorPosBeforeCharacter(0);		
	}
	void TextCursorRenderUnit::SetCursorPosToBeginningOfLine(uintMem characterIndex)
	{
		auto& characterData = textRenderUnit.GetCharacterData();

		if (characterIndex >= characterData.Count())
		{
			SetCursorPosToBeginning();
			return;
		}

		auto& lineData = textRenderUnit.GetLineData();
		auto& line = lineData[characterData[characterIndex].lineIndex];

		cursorCharacterIndex = line.firstCharacterIndex;
		renderDataDirty = true;
		lastTimeCursorSet = TimePoint::GetCurrentWorldTime();
	}
	void TextCursorRenderUnit::SetCursorPosToEnd()
	{
		auto& characterData = textRenderUnit.GetCharacterData();

		if (characterData.Empty())
			SetCursorPosBeforeCharacter(0);
		else
			SetCursorPosBeforeCharacter(characterData.Count() - 1);					
	}
	void TextCursorRenderUnit::SetCursorPosToEndOfLine(uintMem characterIndex)
	{
		auto& characterData = textRenderUnit.GetCharacterData();

		if (characterIndex >= characterData.Count())
		{
			SetCursorPosToEnd();
			return;
		}

		auto& lineData = textRenderUnit.GetLineData();
		auto& line = lineData[characterData[characterIndex].lineIndex];

		cursorCharacterIndex = line.firstCharacterIndex + line.characterCount - 1;
		renderDataDirty = true;
		lastTimeCursorSet = TimePoint::GetCurrentWorldTime();
	}
	bool TextCursorRenderUnit::AdvanceCursor()
	{
		auto& characterData = textRenderUnit.GetCharacterData();

		if (cursorCharacterIndex == characterData.Count() - 1)
			return false;

		++cursorCharacterIndex;
		renderDataDirty = true;
		lastTimeCursorSet = TimePoint::GetCurrentWorldTime();

		return true;
	}
	bool TextCursorRenderUnit::RetreatCursor()
	{		
		CleanData();

		if (cursorCharacterIndex == 0)
			return false;

		--cursorCharacterIndex;
		renderDataDirty = true;
		lastTimeCursorSet = TimePoint::GetCurrentWorldTime();

		return true;
	}	
	uintMem TextCursorRenderUnit::GetIndexOfCharacterAfterCursor()
	{
		CleanData();

		return cursorCharacterIndex;		
	}
	bool TextCursorRenderUnit::IsCursorAtEnd()
	{
		auto& characterData = textRenderUnit.GetCharacterData();

		return cursorCharacterIndex == characterData.Count() - 1;		
	}
	bool TextCursorRenderUnit::IsCursorAtBeggining()
	{
		CleanData();

		return cursorCharacterIndex == 0;
	}
	void TextCursorRenderUnit::ValidateCursorPos()
	{
		auto& characterData = textRenderUnit.GetCharacterData();

		cursorCharacterIndex = std::min(cursorCharacterIndex, characterData.Count());
	}
	void TextCursorRenderUnit::HideCursor()
	{
		shown = false;
	}
	void TextCursorRenderUnit::ShowCursor()
	{
		shown = true;
		lastTimeCursorSet = TimePoint::GetCurrentWorldTime();
	}	
	void TextCursorRenderUnit::OnEvent(const TextRenderUnitBase::RenderDataUpdatedEvent& event)
	{						
		renderDataDirty = true;
	}
}