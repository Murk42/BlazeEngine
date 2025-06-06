#include "pch.h"
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngineGraphics/UI/Common/TextSelection.h"
#include "BlazeEngineCore/Debug/Logger.h"

namespace Blaze::UI
{
	TextSelection::TextSelection(TextContainerBase& textContainer)
		: textContainer(textContainer)
	{
	}
	TextSelection::~TextSelection()
	{
	}
	void TextSelection::Set(uintMem head, uintMem tail)
	{
		uintMem characterCount = textContainer.CharacterCount();

		head = std::min(head, characterCount);
		tail = std::min(tail, characterCount);

		if (this->head == head && this->tail == tail)
			return;

		this->head = head;
		this->tail = tail;

		focusBeginning = std::max(focusBeginning, GetBeginning());
		focusEnd = std::min(focusEnd, GetEnd());

		selectionChangedEventDispatcher.Call({ *this });
	}
	void TextSelection::SetTail(uintMem tail)
	{
		uintMem characterCount = textContainer.CharacterCount();
		tail = std::min(tail, characterCount);

		if (this->tail == tail)
			return;

		this->tail = tail;

		selectionChangedEventDispatcher.Call({ *this });
	}
	void TextSelection::SetHead(uintMem head)
	{
		uintMem characterCount = textContainer.CharacterCount();
		head = std::min(head, characterCount);

		if (this->head == head)
			return;

		this->head = head;

		selectionChangedEventDispatcher.Call({ *this });
	}	
	bool TextSelection::Empty() const
	{
		return head == tail;
	}		
	void TextSelection::FocusWord(uintMem index)
	{
		uintMem begin = index;
		uintMem end = index + 1;

		const uintMem characterCount = textContainer.CharacterCount();

		if (characterCount == 0)
			return;

		if (index >= characterCount)
		{
			BLAZE_ENGINE_GRAPHICS_FATAL("Accessing a text container outside its bounds");
			return;
		}

		uint32 nextChar = textContainer.GetSubString(begin, 1).First().Value();

		if (!isalnum(nextChar))
		{
			focusBeginning = begin;
			focusEnd = focusBeginning + 1;
			Set(focusBeginning, focusEnd);
		}

		enum class WordType {
			AlphaNumeric,
			Numeric
		};

		WordType wordType = [&]() {
			if (isalpha(nextChar)) return WordType::AlphaNumeric;
			else return WordType::Numeric;
			}();

		while (true)
		{
			if (begin == 0)
				break;

			nextChar = textContainer.GetSubString(begin - 1, 1).First().Value();

			if (wordType == WordType::AlphaNumeric)
				if (!isalnum(nextChar))
					break;
			if (wordType == WordType::Numeric)
				if (!isdigit(nextChar))
					if (isalpha(nextChar))
						wordType = WordType::AlphaNumeric;
					else
						break;

			--begin;
		}

		while (true)
		{
			if (end == characterCount)
				break;

			nextChar = textContainer.GetSubString(end, 1).First().Value();

			if (wordType == WordType::AlphaNumeric)
				if (!isalnum(nextChar))
					break;
			if (wordType == WordType::Numeric)
				if (!isdigit(nextChar))
					break;

			++end;
		}

		focusBeginning = begin;
		focusEnd = end;
		Set(focusBeginning, focusEnd);
	}
	void TextSelection::FocusLine(uintMem index)
	{
		uintMem begin = index;
		uintMem end = index;

		const uintMem characterCount = textContainer.CharacterCount();

		while (begin != 0 && textContainer.GetSubString(begin - 1, 1) != "\n") --begin;
		while (end != characterCount && textContainer.GetSubString(end, 1) != "\n") ++end;

		focusBeginning = begin;
		focusEnd = end;
		Set(focusBeginning, focusEnd);
	}
	void TextSelection::TextUpdatedEvent(const TextContainerBase::TextUpdatedEvent& event)
	{
		const uintMem begin = head < tail ? head : tail;
		const uintMem end = head < tail ? tail : head;
		const bool order = head < tail;

		auto SetNew = [&](uintMem begin, uintMem end) {
			if (order)
				Set(begin, end);
			else
				Set(end, begin);
			};

		switch (event.type)
		{
		case TextContainerBase::TextUpdatedEvent::Type::Inserted:
			if (begin >= event.begin + event.count)
				SetNew(begin + event.count, end + event.count);
			else if (begin > event.begin)
				SetNew(event.begin + event.count, end + event.count);
			else if (end > event.begin)
				SetNew(begin, end + event.count);
			break;
		case TextContainerBase::TextUpdatedEvent::Type::Erased:									
			if (begin >= event.begin + event.count)
				SetNew(begin - event.count, end - event.count);
			else if (begin > event.begin)
				SetNew(event.begin, end > event.begin + event.count ? end - event.count : event.begin);
			else if (end > event.begin)
				SetNew(begin, end > event.begin + event.count ? end - event.count : event.begin);
			break;
		}
	}
}
