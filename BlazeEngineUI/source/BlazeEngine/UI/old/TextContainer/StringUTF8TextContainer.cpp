#include "pch.h"
#include "BlazeEngineGraphics/Text/TextContainer/StringUTF8TextContainer.h"

namespace Blaze
{
	StringUTF8TextContainer::StringUTF8TextContainer()
	{
	}
	StringUTF8TextContainer::StringUTF8TextContainer(u8String string)
		: string(string)
	{
	}
	u8StringView StringUTF8TextContainer::GetString() const
	{
		return string;
	}
	u8String StringUTF8TextContainer::SwapStrings(u8String&& newString)
	{
		u8String oldString = std::move(string);

		textUpdatedEventDispatcher.Call({
			.type = TextUpdatedEvent::Type::Erased,
			.begin = 0,
			.count = string.Count(),
			.textContainer = *this
			});

		string = std::move(newString);

		textUpdatedEventDispatcher.Call({
			.type = TextUpdatedEvent::Type::Inserted,
			.begin = 0,
			.count = string.Count(),
				.textContainer = *this
			});

		return oldString;
	}
	void StringUTF8TextContainer::SetString(u8String newString)
	{
		SwapStrings(std::move(newString));
	}

	uintMem StringUTF8TextContainer::CharacterCount() const
	{
		return string.Count();
	}
	u8String StringUTF8TextContainer::GetSubString(uintMem begin, uintMem count) const
	{
		return string.SubString(begin, begin + count);
	}
	UnicodeChar StringUTF8TextContainer::GetCharacter(uintMem index) const
	{
		if (index >= string.Count())
			BLAZE_ENGINE_GRAPHICS_ERROR("Accessing string outside its range");

		return UnicodeChar(string.Ptr() + index, string.Count() - index);
	}
	void StringUTF8TextContainer::EraseSubString(uintMem begin, uintMem count)
	{
		if (count != 0)
		{
			string.Erase(begin, count);

			textUpdatedEventDispatcher.Call({
				.type = TextUpdatedEvent::Type::Erased,
				.begin = begin,
				.count = count,
				.textContainer = *this
				});
		}
	}
	void StringUTF8TextContainer::Insert(uintMem index, u8StringView string)
	{
		if (!string.Empty())
		{
			this->string.Insert(index, string);

			textUpdatedEventDispatcher.Call({
				.type = TextUpdatedEvent::Type::Inserted,
				.begin = index,
				.count = string.Count(),
				.textContainer = *this
				});
		}
	}
}