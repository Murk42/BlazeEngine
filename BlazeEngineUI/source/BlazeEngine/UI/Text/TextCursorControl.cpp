#include "pch.h"
#include "BlazeEngine/UI/Text/TextCursorControl.h"

namespace Blaze::UI::Text
{
	TextCursorControl::TextCursorControl()
		: cursorPos(0), focusBegin(0), focusEnd(0), maxPos(0)
	{
	}
	TextCursorControl::TextCursorControl(uintMem cursorPos, uintMem maxPos)
		: cursorPos(cursorPos), focusBegin(cursorPos), focusEnd(cursorPos), maxPos(maxPos)
	{
	}
	void TextCursorControl::SetMaxPos(uintMem maxPos)
	{
		this->maxPos = maxPos;

		bool changed = false;
		if (cursorPos > maxPos)
		{
			cursorPos = maxPos;
			changed = true;
		}

		if (focusBegin > maxPos)
		{
			focusBegin = maxPos;
			changed = true;
		}

		if (focusEnd > maxPos)
		{
			focusEnd = maxPos;
			changed = true;
		}

		if (changed)
			stateChangedEventDispatcher.Call({ *this });
	}
	void TextCursorControl::ClearSelection()
	{
		focusBegin = focusEnd = cursorPos;
		stateChangedEventDispatcher.Call({ *this });
	}
	void TextCursorControl::SetFocus(uintMem begin, uintMem end)
	{
		if (begin > end)
			std::swap(begin, end);

		end = std::min(end, maxPos);
		begin = std::min(begin, maxPos);

		if (focusBegin == begin && focusEnd == end)
			return;

		focusBegin = begin;
		focusEnd = end;

		stateChangedEventDispatcher.Call({ *this });
	}
	void TextCursorControl::MoveCursorTo(uintMem pos, bool extendSelection)
	{
		pos = std::min(pos, maxPos);

		if (pos == cursorPos && (extendSelection || focusBegin == pos && focusEnd == pos))
			return;

		cursorPos = pos;

		if (!extendSelection)
			focusBegin = focusEnd = cursorPos;

		stateChangedEventDispatcher.Call({ *this });
	}
	uintMem TextCursorControl::GetCursorPos() const
	{
		return cursorPos;
	}
	bool TextCursorControl::IsSelectionEmpty() const
	{
		return focusBegin == focusEnd && cursorPos == focusBegin;
	}
	uintMem TextCursorControl::GetSelectionBeginning() const
	{
		return std::min(cursorPos, focusBegin);
	}
	uintMem TextCursorControl::GetSelectionEnd() const
	{
		return std::max(cursorPos, focusEnd);
	}
}