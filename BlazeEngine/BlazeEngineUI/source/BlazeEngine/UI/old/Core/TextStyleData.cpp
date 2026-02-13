#include "pch.h"
#include "BlazeEngineGraphics/Text/Core/TextStyleData.h"

namespace Blaze
{
	TextStyleData::TextStyleData()
	{
	}
	void TextStyleData::ClearAllStyles()
	{
		spans.Clear();
	}
	void TextStyleData::SetStyle(uintMem startIndex, uintMem endIndex, ResourceRef<TextStyle> style)
	{
		if (endIndex <= startIndex)
		{
			if (endIndex < startIndex)
				BLAZE_ENGINE_GRAPHICS_ERROR("Invalid index range, end is smaller that start");

			return;
		}

		if (style.IsNull())
			ClearStyle(startIndex, endIndex);

		uintMem index = std::lower_bound(spans.FirstIterator(), spans.BehindIterator(), startIndex, [](const TextStyleSpan& it, const uintMem& value) {
			return it.endIndex < value;
			}) - spans.FirstIterator();

		if (index == spans.Count())
		{
			//There are no spans or all of the spans are before the range (without touching)
			spans.AddBack(TextStyleSpan{ style, startIndex, endIndex });			
		}
		else
		{
			//Next span is either intersecting the range or is completely right of it

			if (spans[index].startIndex < startIndex)
				if (spans[index].style == style)
				{
					if (spans[index].endIndex < endIndex)
					{
						spans[index].endIndex = endIndex;
						++index;
					}
				}
				else
				{
					uintMem previousSpanEnd = spans[index].endIndex;
					spans[index].endIndex = startIndex;
					spans.AddAt(index + 1, TextStyleSpan{ style, startIndex, endIndex });

					if (spans[index].endIndex > endIndex)
					{
						spans.AddAt(index + 2, TextStyleSpan{ spans[index].style, endIndex, spans[index].endIndex });						
					}
					else
						index += 2;
				}

			//Next span startIndex is between startIndex and endIndex (inclusive for both)
			//The remaining spans need to be removed or shortened to be valid

			while (index < spans.Count() && spans[index].startIndex <= endIndex)
			{
				if (spans[index].endIndex <= endIndex)
				{
					spans.EraseAt(index); //Erase the span if it is completely inside the range
					++index;
				}
				else if (spans[index].style == style)
				{
					spans[index - 1].endIndex = spans[index].endIndex;

					spans.EraseAt(index);
					break;
				}
				else
				{
					spans[index].startIndex = endIndex;
					break;
				}
			}
		}

		textStyleSpanChangedEventDispathcer.Call({
				.type = TextStyleSpanChangedEvent::Type::StyleChange,
				.startIndex = startIndex,
				.endIndex = endIndex,
				.style = style,
				.textStyleData = *this
			});
	}
	void TextStyleData::ClearStyle(uintMem startIndex, uintMem endIndex)
	{
		auto index = std::lower_bound(spans.FirstIterator(), spans.BehindIterator(), startIndex, [](const TextStyleSpan& it, const uintMem& value) {
			return it.endIndex < value;
			}) - spans.FirstIterator();

		if (index == spans.Count() || spans[index].startIndex >= endIndex)
			return;

		//Next span is either intersecting the range or is completely right of it

		if (spans[index].startIndex < startIndex)
		{
			uintMem previousSpanEnd = spans[index].endIndex;
			spans[index].endIndex = startIndex;

			if (spans[index].endIndex > endIndex)
			{
				spans.AddAt(index + 1, TextStyleSpan{ spans[index].style, endIndex, spans[index].endIndex });
				return;
			}

			if (spans[index].endIndex == endIndex)
				return;

			index += 1;
		}

		//Next span startIndex is between startIndex and endIndex (inclusive for both)
		//The remaining spans need to be removed or shortened to be valid

		while (index < spans.Count() && spans[index].startIndex <= endIndex)
		{
			if (spans[index].endIndex <= endIndex)
			{
				spans.EraseAt(index); //Erase the span if it is completely inside the range
				++index;
			}
			else
			{
				spans[index].startIndex = endIndex;
				return;
			}
		}

		textStyleSpanChangedEventDispathcer.Call({
				.type = TextStyleSpanChangedEvent::Type::StyleChange,
				.startIndex = startIndex,
				.endIndex = endIndex,
				.style = { },
				.textStyleData = *this
			});
	}
	void TextStyleData::EraseSubString(uintMem startIndex, uintMem endIndex)
	{
		ClearStyle(startIndex, endIndex);

		uintMem index = std::lower_bound(spans.FirstIterator(), spans.BehindIterator(), endIndex, [](const TextStyleSpan& it, const uintMem& value) {
			return it.startIndex < value;
			}) - spans.FirstIterator();

		const uintMem rangeSize = endIndex - startIndex;
		
		if (index != 0 && spans[index - 1].endIndex == startIndex && spans[index].startIndex == endIndex)
		{
			spans[index - 1].endIndex = spans[index].endIndex - rangeSize;
			spans.EraseAt(index);
		}

		for (; index != spans.Count(); ++index)
		{
			spans[index].startIndex -= rangeSize;
			spans[index].endIndex -= rangeSize;
		}

		textStyleSpanChangedEventDispathcer.Call({
				.type = TextStyleSpanChangedEvent::Type::SubStringErased,
				.startIndex = startIndex,
				.endIndex = endIndex,
				.style = { },
				.textStyleData = *this
			});
	}
	void TextStyleData::InsertSubString(uintMem characterIndex, uintMem size)
	{
		uintMem index = std::lower_bound(spans.FirstIterator(), spans.BehindIterator(), characterIndex + 1, [](const TextStyleSpan& it, const uintMem& value) {
			return it.endIndex < value;
			}) - spans.FirstIterator();

		if (index == spans.Count() || spans[index].startIndex > characterIndex || spans[index].endIndex <= characterIndex)
			return;

		auto newSubStringStyle = spans[index].style;

		spans[index].endIndex += size;

		for (; index < spans.Count(); ++index)
		{
			spans[index].startIndex += size;
			spans[index].endIndex += size;
		}

		textStyleSpanChangedEventDispathcer.Call({
				.type = TextStyleSpanChangedEvent::Type::SubStringInserted,
				.startIndex = characterIndex,
				.endIndex = characterIndex + size,
				.style = newSubStringStyle,
				.textStyleData = *this
			});
	}
	auto TextStyleData::FindStyleSpanAt(uintMem characterIndex) const -> ConstIterator
	{
		auto it = std::lower_bound(spans.FirstIterator(), spans.BehindIterator(), characterIndex, [](const TextStyleSpan& it, const uintMem& value) {
			return it.endIndex < value;
			});

		if (it == spans.BehindIterator() || it->startIndex > characterIndex)
			return { };

		return it;
	}
	auto TextStyleData::FindFirstStyleSpanStartingAt(uintMem characterIndex) const -> ConstIterator
	{
		return std::lower_bound(spans.FirstIterator(), spans.BehindIterator(), characterIndex, [](const TextStyleSpan& it, const uintMem& value) {
			return it.endIndex < value;
			});
	}
}