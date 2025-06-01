#include "pch.h"
#include "BlazeEngineGraphics/UI/Common/TextCursor.h"

namespace Blaze::UI
{
	TextCursor::TextCursor(TextSelection& selection)
		: selection(selection), cursorPos(0)
	{
		selection.textContainer.textUpdatedEventDispatcher.AddHandler<&TextCursor::TextUpdatedEvent>(*this);
		selection.selectionChangedEventDispatcher.AddHandler<&TextCursor::SelectionChangedEvent>(*this);
	}
	TextCursor::~TextCursor()
	{
		selection.textContainer.textUpdatedEventDispatcher.RemoveHandler<&TextCursor::TextUpdatedEvent>(*this);
		selection.selectionChangedEventDispatcher.RemoveHandler<&TextCursor::SelectionChangedEvent>(*this);
	}
	void TextCursor::SetCursorPos(uintMem newCursorPos)
	{
		newCursorPos = std::min(newCursorPos, selection.textContainer.CharacterCount());
		
		if (cursorPos == newCursorPos)
			return;

		cursorPos = newCursorPos;

		selection.Set(cursorPos, cursorPos);

		cursorMovedEventDispatcher.Call({
			.cursor = *this
			});
	}
	void TextCursor::TextUpdatedEvent(const TextContainerBase::TextUpdatedEvent& event)
	{
		switch (event.type)
		{
		case TextContainerBase::TextUpdatedEvent::Type::Inserted:
			if (cursorPos >= event.begin)
				SetCursorPos(cursorPos + event.count);
			break;
		case TextContainerBase::TextUpdatedEvent::Type::Erased:
			if (cursorPos > event.begin + event.count)
				SetCursorPos(cursorPos - event.count);
			else if (cursorPos > event.begin)
				SetCursorPos(event.begin);
			break;		
		}
	}
	void TextCursor::SelectionChangedEvent(const TextSelection::SelectionChangedEvent& event)
	{
		uintMem newCursorPos = selection.GetTail();

		if (cursorPos == newCursorPos)
			return;

		cursorPos = newCursorPos;

		cursorMovedEventDispatcher.Call({
			.cursor = *this
			});		
	}
}