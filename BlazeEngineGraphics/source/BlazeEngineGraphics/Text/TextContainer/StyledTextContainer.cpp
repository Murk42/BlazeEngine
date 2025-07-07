#include "pch.h"
#include "BlazeEngineGraphics/Text/TextContainer/StyledTextContainer.h"

namespace Blaze
{
	StyledTextContainer::StyledTextContainer(TextContainerBase& container)
		: TextContainerReference(container)
	{
		textUpdatedEventDispatcher.AddHandler<&StyledTextContainer::TextUpdatedEvent>(*this);
	}
	StyledTextContainer::~StyledTextContainer()
	{
		textUpdatedEventDispatcher.RemoveHandler<&StyledTextContainer::TextUpdatedEvent>(*this);
	}
	void StyledTextContainer::AddStyleSpan(uintMem startIndex, uintMem endIndex, ResourceRef<TextStyle> style)
	{
		auto it = std::lower_bound(spans.FirstIterator(), spans.BehindIterator(), startIndex, [](const TextStyleSpan& it, const uintMem& value) {
			return it.endIndex < value;
			});

		if (it == spans.BehindIterator())
		{			
			if (!spans.Empty() && (it - 1)->endIndex == startIndex && (it - 1)->style == style)
				(it - 1)->endIndex = endIndex;
			else				
				spans.AddAt(it, TextStyleSpan{ style,  startIndex, endIndex });
		}
		else if (it->style == style)
		{
			it->endIndex = endIndex;

			auto next = it + 1;
			if (next != spans.BehindIterator() && next->startIndex <= endIndex)
			{
				if (next->style == style)
				{
					it->endIndex == next->endIndex;
					spans.EraseAt(next);
				}
				else				
					next->startIndex = endIndex;
			}
		}
		else
		{			
			it->endIndex = startIndex;
					
			auto next = it + 1;
			if (next != spans.BehindIterator() && next->startIndex <= endIndex)
			{
				if (next->style == style)				
					it->startIndex == startIndex;				
				else
				{
					next->startIndex = endIndex;
					spans.AddAt(it, TextStyleSpan{ style,  startIndex, endIndex });
				}
			}
			else
				spans.AddAt(it, TextStyleSpan{ style,  startIndex, endIndex });
		}
	}
	Array<TextStyleSpan>::ConstIterator StyledTextContainer::FindStyleSpanAt(uintMem characterIndex) const
	{
		auto it = std::upper_bound<Array<TextStyleSpan>::ConstIterator, uintMem>(spans.FirstIterator(), spans.BehindIterator(), characterIndex, [](const uintMem& value, const TextStyleSpan& it) {
			return it.endIndex < value;
			});

		if (it == spans.BehindIterator() || it->startIndex > characterIndex)
			return { };

		return it;
	}
	Array<TextStyleSpan>::ConstIterator StyledTextContainer::FindFirstStyleSpanStartingAt(uintMem characterIndex) const
	{
		return std::upper_bound(spans.FirstIterator(), spans.BehindIterator(), characterIndex, [](const uintMem& value, const TextStyleSpan& it) {
			return it.endIndex < value;
			});		
	}
	ArrayView<TextStyleSpan> StyledTextContainer::GetAllStyleSpans() const
	{
		return spans;
	}
	Array<TextStyleSpan>::Iterator StyledTextContainer::FindStyleSpanAt(uintMem characterIndex)
	{
		auto it = std::upper_bound(spans.FirstIterator(), spans.BehindIterator(), characterIndex, [](const uintMem& value, const TextStyleSpan& it) {
			return it.endIndex < value;
			});

		if (it == spans.BehindIterator() || it->startIndex > characterIndex)
			return { };

		return it;
	}
	Array<TextStyleSpan>::Iterator StyledTextContainer::FindFirstStyleSpanStartingAt(uintMem characterIndex)
	{
		return std::upper_bound(spans.FirstIterator(), spans.BehindIterator(), characterIndex, [](const uintMem& value, const TextStyleSpan& it) {
			return it.endIndex < value;
			});
	}
	void StyledTextContainer::TextUpdatedEvent(const TextContainerBase::TextUpdatedEvent& event)
	{		
		switch (event.type)
		{
		case TextContainerBase::TextUpdatedEvent::Type::Inserted: {
			auto it = FindStyleSpanAt(event.begin);

			if (!it.IsNull())
			{
				it->endIndex += event.count;

				++it;
				while (it != spans.BehindIterator())
				{
					it->startIndex += event.count;
					it->endIndex += event.count;
					++it;
				}
			}
			break;
		}
		case TextContainerBase::TextUpdatedEvent::Type::Erased: {
			auto it = FindFirstStyleSpanStartingAt(event.begin);

			if (it == spans.BehindIterator())
				break;

			if (it->startIndex < event.begin)
			{
				if (it->endIndex > event.begin + event.count)
					it->endIndex -= event.count;
				else
					it->endIndex = event.begin;

				++it;
			}

			uintMem beginIndex = it - spans.FirstIterator();
			uintMem endIndex = beginIndex;

			while (endIndex < spans.Count() && spans[endIndex].endIndex <= event.begin + event.count)
				++endIndex;

			if (endIndex != spans.Count() && spans[endIndex].startIndex <= event.begin + event.count)
			{
				if (spans[endIndex].style == it->style)
				{
					it->endIndex = spans[endIndex].endIndex;
					++endIndex;
				}
				else
					spans[endIndex].startIndex = event.begin;
			}

			for (uintMem i = beginIndex; i < endIndex; ++i)
				spans.EraseAt(i);

			break;
		}
		default:
			break;
		}
	}
}