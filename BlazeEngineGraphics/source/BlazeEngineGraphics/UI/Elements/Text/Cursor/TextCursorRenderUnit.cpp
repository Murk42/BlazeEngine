#include "pch.h"
#include "BlazeEngineGraphics/UI/Elements/Text/Cursor/TextCursorRenderUnit.h"

namespace Blaze::UIGraphics
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
	static Rectf GetCullingRect(const UI::NodeFinalTransform& transfrom, Vec2f position, float scale, float rotation, Vec2f right, Vec2f up)
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

	TextCursorRenderUnitBase::TextCursorRenderUnitBase(StringView rendererName)
		: RenderUnit(rendererName)
	{		
	}
	TextCursorRenderUnit::TextCursorRenderUnit(UIGraphics::TextRenderUnitBase& textRenderUnit) :
		TextCursorRenderUnitBase("TexturedRectRenderer"), textRenderUnit(textRenderUnit), cursorLineIndex(0), cursorLineCharacterIndex(0), renderDataDirty(false), rendered(true), shown(false)
	{		
		textRenderUnit.renderDataUpdatedEventDispatcher.AddHandler(*this);

		renderData.blend = 1;
		renderData.color = 0xffffffff;
		renderData.alpha = 1.0f;
		renderData.texture = nullptr;
		renderData.uv1 = Vec2f(0, 0);
		renderData.uv2 = Vec2f(0, 0);
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

		auto& linesData = textRenderUnit.GetLineData();

		//If there are no lines or the line or character index aren't valid, don't render anything. This should never happen, because the text render unit will always have one
		//line, even if it is empty, and the indicies are checked if they are valid in the SetCursorPos function
		if (linesData.Empty() || cursorLineIndex >= linesData.Count() || cursorLineCharacterIndex > linesData[cursorLineIndex].characterCount)
		{
			culled = true;
			return true;
		}

		auto& line = linesData[cursorLineIndex];
		auto& characterData = textRenderUnit.GetCharacterData();
		uint cursorCharacterIndex = cursorLineCharacterIndex + line.firstCharacterIndex;
		Vec2f cursorSize = Vec2f(0.05f, 0.8f) * textRenderUnit.GetPixelFontHeight();
		cursorSize = Vec2f(std::round(cursorSize.x), std::round(cursorSize.y));
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
		else if (cursorLineCharacterIndex == line.characterCount)
		{
			//The cursor is behind all characters in the line
			rect.pos.x = characterData[cursorCharacterIndex - 1].pos.x + characterData[cursorCharacterIndex - 1].size.x;
		}
		else
		{
			//The cursor is between the characters in the line
			rect.pos.x = (characterData[cursorCharacterIndex - 1].pos.x + characterData[cursorCharacterIndex - 1].size.x + characterData[cursorCharacterIndex].pos.x - cursorSize.x) / 2;
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
	void TextCursorRenderUnit::SetCursorPos(uintMem lineIndex, uintMem lineCharacterIndex)
	{
		if (cursorLineIndex != lineIndex || cursorLineCharacterIndex != lineCharacterIndex)
		{
			cursorLineIndex = lineIndex;
			cursorLineCharacterIndex = lineCharacterIndex;

			ValidateCursorPos();

			renderDataDirty = true;
			lastTimeCursorSet = TimePoint::GetCurrentWorldTime();
		}
	}
	void TextCursorRenderUnit::SetCursorPosBeforeCharacter(uintMem characterIndex)
	{
		auto& characterData = textRenderUnit.GetCharacterData();

		if (characterIndex >= characterData.Count())
		{
			SetCursorPosToEnd();
			return;
		}

		auto& lineData = textRenderUnit.GetLineData();
		auto& line = lineData[characterData[characterIndex].lineIndex];

		uintMem newCursorLineIndex = characterData[characterIndex].lineIndex;
		uintMem newCursorLineCharacterIndex = characterIndex - textRenderUnit.GetLineData()[newCursorLineIndex].firstCharacterIndex;

		SetCursorPos(newCursorLineIndex, newCursorLineCharacterIndex);
	}
	void TextCursorRenderUnit::SetCursorPosAfterCharacter(uintMem characterIndex)
	{
		auto& characterData = textRenderUnit.GetCharacterData();

		if (characterIndex >= characterData.Count())
		{
			SetCursorPosToEnd();
			return;
		}

		auto& lineData = textRenderUnit.GetLineData();
		auto& line = lineData[characterData[characterIndex].lineIndex];

		uintMem newCursorLineIndex = characterData[characterIndex].lineIndex;
		uintMem newCursorLineCharacterIndex = characterIndex - textRenderUnit.GetLineData()[newCursorLineIndex].firstCharacterIndex + 1;

		SetCursorPos(newCursorLineIndex, newCursorLineCharacterIndex);
	}
	void TextCursorRenderUnit::SetCursorPosToBeginning()
	{
		SetCursorPos(0, 0);
	}
	void TextCursorRenderUnit::SetCursorPosToEnd()
	{
		if (textRenderUnit.GetLineData().Empty())
			SetCursorPos(0, 0);
		else
			SetCursorPos(textRenderUnit.GetLineData().Count() - 1, textRenderUnit.GetLineData().Last().characterCount);
	}
	bool TextCursorRenderUnit::AdvanceCursor()
	{
		CleanData();

		auto& lineData = textRenderUnit.GetLineData();

		if (cursorLineCharacterIndex == lineData[cursorLineIndex].characterCount)
		{
			if (cursorLineIndex == lineData.Count() - 1)
				return false;
			else
			{
				auto& characterData = textRenderUnit.GetCharacterData();
				auto& currentLine = lineData[cursorLineIndex];
				uintMem indexOfCharacterAfterCurrentLine = currentLine.firstCharacterIndex + currentLine.characterCount;

				if (characterData[indexOfCharacterAfterCurrentLine].character != '\n')
					SetCursorPos(cursorLineIndex + 1, 1);
				else
					SetCursorPos(cursorLineIndex + 1, 0);
			}
		}
		else
			SetCursorPos(cursorLineIndex, cursorLineCharacterIndex + 1);

		return true;
	}
	bool TextCursorRenderUnit::RetreatCursor()
	{
		auto& lineData = textRenderUnit.GetLineData();

		if (cursorLineCharacterIndex == 0)
		{
			if (cursorLineIndex == 0)
				return false;
			else
				SetCursorPos(cursorLineIndex - 1, lineData[cursorLineIndex - 1].characterCount);
		}
		else
		{
			if (cursorLineIndex != 0)
			{
				auto& characterData = textRenderUnit.GetCharacterData();
				auto& previousLine = lineData[cursorLineIndex - 1];
				uintMem indexOfCharacterAfterPreviousLine = previousLine.firstCharacterIndex + previousLine.characterCount;

				if (cursorLineCharacterIndex == 1 && characterData[indexOfCharacterAfterPreviousLine].character != '\n')
					SetCursorPos(cursorLineIndex - 1, previousLine.characterCount);
				else
					SetCursorPos(cursorLineIndex, cursorLineCharacterIndex - 1);
			}
			else
				SetCursorPos(cursorLineIndex, cursorLineCharacterIndex - 1);
		}

		return true;
	}
	uintMem TextCursorRenderUnit::GetCursorLineIndex()
	{
		CleanData();
		return cursorLineIndex;
	}
	uintMem TextCursorRenderUnit::GetCursorLineCharacterIndex()
	{
		CleanData();
		return cursorLineCharacterIndex;
	}
	uintMem TextCursorRenderUnit::GetIndexOfCharacterAfterCursor()
	{
		CleanData();

		auto& lineData = textRenderUnit.GetLineData();

		if (lineData.Empty())
			return 0;

		if (cursorLineIndex >= lineData.Count())
			return textRenderUnit.GetCharacterData().Count() - 1;

		if (cursorLineCharacterIndex > lineData[cursorLineIndex].characterCount)
			if (cursorLineIndex == lineData.Count() - 1)
				if (textRenderUnit.GetCharacterData().Count() == 0)
					return 0;
				else
					return textRenderUnit.GetCharacterData().Count() - 1;
			else
				return lineData[cursorLineIndex + 1].firstCharacterIndex;
		else
			return lineData[cursorLineIndex].firstCharacterIndex + cursorLineCharacterIndex;
	}
	uintMem TextCursorRenderUnit::GetIndexOfCharacterBeforeCursor()
	{
		CleanData();

		auto& lineData = textRenderUnit.GetLineData();

		if (lineData.Empty())
			return 0;

		if (cursorLineIndex >= lineData.Count())
			return textRenderUnit.GetCharacterData().Count() - 1;

		if (cursorLineCharacterIndex >= lineData[cursorLineIndex].characterCount)
			return lineData[cursorLineIndex].firstCharacterIndex + lineData[cursorLineIndex].characterCount - 1;
		else if (cursorLineCharacterIndex == 0)
		{
			if (cursorLineIndex == 0)
				return 0;
			else
				return lineData[cursorLineIndex - 1].firstCharacterIndex + lineData[cursorLineIndex - 1].characterCount;
		}
		else
			return lineData[cursorLineIndex].firstCharacterIndex + cursorLineCharacterIndex - 1;
	}
	bool TextCursorRenderUnit::IsCursorAtEnd()
	{
		auto& lineData = textRenderUnit.GetLineData();

		if (lineData.Empty())
			return true;

		if (cursorLineIndex >= lineData.Count())
			return true;

		if (cursorLineIndex == lineData.Count() - 1)
			return cursorLineCharacterIndex == lineData[cursorLineIndex].characterCount;

		return false;
	}
	bool TextCursorRenderUnit::IsCursorAtBeggining()
	{
		auto& lineData = textRenderUnit.GetLineData();

		if (lineData.Empty())
			return true;

		if (cursorLineIndex >= lineData.Count())
			return false;

		if (cursorLineIndex == 0)
			return cursorLineCharacterIndex == 0;

		return false;
	}
	void TextCursorRenderUnit::ValidateCursorPos()
	{
		auto& lineData = textRenderUnit.GetLineData();

		cursorLineIndex = std::min(cursorLineIndex, lineData.Count() - 1);

		//Wrap the cursor if the cursor is outside the line and the next line is wrapped
		//while (cursorLineCharacterIndex > lineData[cursorLineIndex].characterCount && lineData.Count() > cursorLineIndex + 1 && lineData[cursorLineIndex + 1].wrapped)
		//{
		//	cursorLineCharacterIndex -= lineData[cursorLineIndex].characterCount;
		//	++cursorLineIndex;
		//}

		cursorLineCharacterIndex = std::min(cursorLineCharacterIndex, lineData[cursorLineIndex].characterCount);

		if (cursorLineIndex != 0 && cursorLineCharacterIndex == 0)
		{
			auto& characterData = textRenderUnit.GetCharacterData();
			auto& previousLine = lineData[cursorLineIndex - 1];
			uintMem indexOfCharacterAfterPreviousLine = previousLine.firstCharacterIndex + previousLine.characterCount;

			if (characterData[indexOfCharacterAfterPreviousLine].character != '\n')
			{
				--cursorLineIndex;
				cursorLineCharacterIndex = previousLine.characterCount;
			}
		}
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
	void TextCursorRenderUnit::OnEvent(UIGraphics::TextRenderUnitBase::RenderDataUpdatedEvent event)
	{						
		renderDataDirty = true;
	}
}