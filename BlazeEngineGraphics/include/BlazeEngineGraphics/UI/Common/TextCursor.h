#pragma once
#include "BlazeEngineGraphics/UI/Common/TextContainerBase.h"
#include "BlazeEngineGraphics/UI/Common/TextSelection.h"

namespace Blaze::UI
{
	class TextCursor
	{
	public:
		struct CursorMovedEvent
		{
			TextCursor& cursor;
		};
		EventDispatcher<CursorMovedEvent> cursorMovedEventDispatcher;
		
		TextSelection& selection;

		TextCursor(TextSelection& selection);
		~TextCursor();

		void SetCursorPos(uintMem newCursorPos);		

		inline uintMem GetCursorPos() const { return cursorPos; }
	private:
		uintMem cursorPos;

		void TextUpdatedEvent(const TextContainerBase::TextUpdatedEvent& event);
		void SelectionChangedEvent(const TextSelection::SelectionChangedEvent& event);
	};
}