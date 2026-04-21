#pragma once

namespace Blaze::UI::Text
{
	class TextCursorControl
	{
	public:
		struct StateChangedEvent
		{
			TextCursorControl& cursorControl;
		};
		mutable EventDispatcher<StateChangedEvent> stateChangedEventDispatcher;

		TextCursorControl();		
		TextCursorControl(uintMem cursorPos, uintMem maxPos);

		void SetMaxPos(uintMem maxPos);

		void ClearSelection();	
		void SetFocus(uintMem begin, uintMem end);
		void MoveCursorTo(uintMem pos, bool extendSelection);

		uintMem GetCursorPos() const;
		bool IsSelectionEmpty() const;
		uintMem GetSelectionBeginning() const;
		uintMem GetSelectionEnd() const;
	private:
		uintMem cursorPos;
		uintMem focusBegin;
		uintMem focusEnd;
		uintMem maxPos;
	};
}