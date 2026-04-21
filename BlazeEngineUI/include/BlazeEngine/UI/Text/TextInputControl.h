#pragma once
#include "BlazeEngine/Core/String/String.h"
#include "BlazeEngine/Core/Container/Array.h"
#include "BlazeEngine/UI/Text/TextCursorControl.h"

namespace Blaze::UI::Text
{
	class TextInputControl
	{
	public:	
		TextInputControl();
		TextInputControl(u8String text);
		TextInputControl(u8String text, uintMem cursorCharacterPos);

		void SetText(u8String text);
		inline u8StringView GetText() const { return text; }

		void SetFocus(uintMem begin, uintMem end);
		void MoveCursorTo(uintMem newCursorCharacterPos, bool expandSelection);

		void InsertAtCursor(u8StringView string);
		void EraseSelection();
		void EraseBeforeCursor();
		void EraseAfterCursor();

		uintMem GetCursorPos() const;

		bool IsSelectionEmpty() const;
		uintMem GetSelectionBeginning() const;
		uintMem GetSelectionEnd() const;

		inline const TextCursorControl& GetCursorControl() const { return cursorControl; }
	private:
		TextCursorControl cursorControl;
		
		u8String text;
		Array<uintMem> graphemePositions;

		uintMem MapGraphemeToCharacterPos(uintMem graphemePos) const;
	};
}