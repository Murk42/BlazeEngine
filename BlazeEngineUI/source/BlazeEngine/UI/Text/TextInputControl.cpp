#include "pch.h"
#include "BlazeEngine/UI/Text/TextInputControl.h"
#include <utf8proc.h>

namespace Blaze::UI::Text
{
	static bool GetGraphemePositions(const u8StringView& text, Array<uintMem>& graphemeCharacterIndices)
	{
		if (text.Empty())
		{
			graphemeCharacterIndices.Clear();
			return true;
		}

		graphemeCharacterIndices.ReserveExactly(text.Count());

		utf8proc_int32_t state = 0;
		uintMem offset = 0;

		UnicodeChar prev;

		uintMem characterUTF8Size = prev.FromFirstCodePoints(text);
		if (characterUTF8Size == 0)
		{
			BLAZE_LOG_ERROR("Invalid UTF-8 sequence");
			graphemeCharacterIndices.Clear();
			return false;
		}

		graphemeCharacterIndices.AddBack(0);
		offset += characterUTF8Size;

		while (offset < text.Count())
		{
			UnicodeChar curr;

			characterUTF8Size = curr.FromFirstCodePoints(text.SubString(offset));
			if (characterUTF8Size == 0)
			{
				BLAZE_LOG_ERROR("Invalid UTF-8 sequence");
				graphemeCharacterIndices.Clear();
				return false;
			}

			if (utf8proc_grapheme_break_stateful(static_cast<utf8proc_int32_t>(prev.Value()), static_cast<utf8proc_int32_t>(curr.Value()), &state))
				graphemeCharacterIndices.AddBack(offset);

			offset += characterUTF8Size;

			prev = curr;
		}

		return true;
	}

	TextInputControl::TextInputControl()
	{
	}
	TextInputControl::TextInputControl(u8String text)
	{
		SetText(text);
	}
	TextInputControl::TextInputControl(u8String text, uintMem cursorCharacterPos)
	{
		SetText(text);
		MoveCursorTo(cursorCharacterPos, false);
	}

	void TextInputControl::SetText(u8String newText)
	{
		if (text == newText)
			return;

		text = std::move(newText);

		if (!GetGraphemePositions(text, graphemePositions))
		{
			BLAZE_LOG_ERROR("Failed to get grapheme positions for text");
			text.Clear();
		}

		cursorControl.SetMaxPos(graphemePositions.Count());
	}
	void TextInputControl::SetFocus(uintMem begin, uintMem end)
	{
		uintMem beginGrapheme = std::lower_bound(graphemePositions.Begin(), graphemePositions.End(), begin) - graphemePositions.Begin();
		uintMem endGrapheme = std::lower_bound(graphemePositions.Begin(), graphemePositions.End(), end) - graphemePositions.Begin();

		cursorControl.SetFocus(beginGrapheme, endGrapheme);
	}
	void TextInputControl::MoveCursorTo(uintMem newCursorCharacterPos, bool expandSelection)
	{		
		uintMem pos = std::lower_bound(graphemePositions.Begin(), graphemePositions.End(), newCursorCharacterPos) - graphemePositions.Begin();		

		cursorControl.MoveCursorTo(pos, expandSelection);
	}
	void TextInputControl::InsertAtCursor(u8StringView string)
	{
		u8String newString = text;

		uintMem begin = MapGraphemeToCharacterPos(cursorControl.GetSelectionBeginning());
		uintMem oldGraphemeCount = graphemePositions.Count();

		if (cursorControl.GetSelectionBeginning() != cursorControl.GetSelectionEnd())
		{
			uintMem end = MapGraphemeToCharacterPos(cursorControl.GetSelectionEnd());
		
			newString.Erase(begin, end - begin);
		}

		newString.Insert(begin, string);

		SetText(std::move(newString));
				
		cursorControl.MoveCursorTo(cursorControl.GetSelectionEnd() + graphemePositions.Count() - oldGraphemeCount, false);
	}
	void TextInputControl::EraseSelection()
	{
		if (cursorControl.GetSelectionBeginning() != cursorControl.GetSelectionEnd())
		{
			uintMem begin = MapGraphemeToCharacterPos(cursorControl.GetSelectionBeginning());
			uintMem end = MapGraphemeToCharacterPos(cursorControl.GetSelectionEnd());

			u8String newString = text;
			newString.Erase(begin, end - begin);

			cursorControl.MoveCursorTo(cursorControl.GetSelectionBeginning(), false);
			SetText(std::move(newString));

		}
	}
	void TextInputControl::EraseBeforeCursor()
	{
		if (cursorControl.GetSelectionBeginning() == 0)
			return;

		uintMem begin = MapGraphemeToCharacterPos(cursorControl.GetSelectionBeginning() - 1);
		uintMem end = MapGraphemeToCharacterPos(cursorControl.GetSelectionBeginning());

		u8String newString = text;
		newString.Erase(begin, end - begin);

		cursorControl.MoveCursorTo(cursorControl.GetSelectionBeginning() - 1, false);
		SetText(std::move(newString));
	}
	void TextInputControl::EraseAfterCursor()
	{
		if (cursorControl.GetSelectionBeginning() == graphemePositions.Count())
			return;

		uintMem begin = MapGraphemeToCharacterPos(cursorControl.GetSelectionBeginning());
		uintMem end = MapGraphemeToCharacterPos(cursorControl.GetSelectionBeginning() + 1);

		u8String newString = text;
		newString.Erase(begin, end - begin);

		cursorControl.MoveCursorTo(cursorControl.GetSelectionBeginning(), false);
		SetText(std::move(newString));
	}
	uintMem TextInputControl::GetCursorPos() const
	{
		return MapGraphemeToCharacterPos(cursorControl.GetCursorPos());
	}
	bool TextInputControl::IsSelectionEmpty() const
	{
		return cursorControl.IsSelectionEmpty();
	}
	uintMem TextInputControl::GetSelectionBeginning() const
	{
		uintMem graphemeSelectionBeginning = cursorControl.GetSelectionBeginning();

		if (graphemeSelectionBeginning >= graphemePositions.Count())
			return text.Count();
		else
			return graphemePositions[graphemeSelectionBeginning];
	}
	uintMem TextInputControl::GetSelectionEnd() const
	{
		uintMem graphemeSelectionEnd = cursorControl.GetSelectionEnd();

		if (graphemeSelectionEnd >= graphemePositions.Count())
			return text.Count();
		else
			return graphemePositions[graphemeSelectionEnd];
	}
	uintMem TextInputControl::MapGraphemeToCharacterPos(uintMem graphemePos) const
	{
		if (graphemePos > graphemePositions.Count())
		{
			BLAZE_LOG_FATAL("Invalid grapheme pos");
			return 0;
		}

		if (graphemePos == graphemePositions.Count())
			return text.Count();

		return graphemePositions[graphemePos];
	}
}