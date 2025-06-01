#pragma once
#include "BlazeEngineGraphics/UI/Common/TextContainerBase.h"

namespace Blaze::UI
{
	class TextSelection
	{
	public:
		struct SelectionChangedEvent
		{
			TextSelection& selection;
		};
		EventDispatcher<SelectionChangedEvent> selectionChangedEventDispatcher;

		TextContainerBase& textContainer;

		TextSelection(TextContainerBase& textContainer);
		~TextSelection();

		/*
			Sets the selection so that characters with index in range [head, tail) (if 'head' <= 'tail') or 
			[tail, head) (if 'head' > 'tail') are selected. Notice the inclusive brackets. The order of 'tail' and 
			'head' is important because the 'tail' value will be changed when the selection expands or shrinks
		*/
		void Set(uintMem head, uintMem tail);
		void SetTail(uintMem index);
		void SetHead(uintMem index);
		void FocusWord(uintMem index);
		void FocusLine(uintMem index);

		bool Empty() const;		

		inline uintMem GetHead() const { return head; }
		inline uintMem GetTail() const { return tail; }		
		inline uintMem GetBeginning() const { return std::min(head, tail); }
		inline uintMem GetEnd() const { return std::max(head, tail); }
		inline uintMem GetSize() const { return tail > head ? tail - head : head - tail; }
		inline uintMem GetFocusBeginning() const { return focusBeginning; }
		inline uintMem GetFocusEnd() const { return focusEnd; }
		inline uintMem GetFocusSize() const { return focusEnd - focusBeginning; }
	private:
		uintMem head;
		uintMem tail;
		uintMem focusBeginning;
		uintMem focusEnd;

		void TextUpdatedEvent(const TextContainerBase::TextUpdatedEvent& event);
	};
}