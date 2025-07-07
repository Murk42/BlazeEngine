#include "pch.h"
#include "BlazeEngineGraphics/Text/TextContainer/StringUTF8TextContainer.h"

namespace Blaze
{
	StringUTF8TextContainer::StringUTF8TextContainer()
	{
	}
	StringUTF8TextContainer::StringUTF8TextContainer(StringUTF8 string)
		: string(string)
	{
	}
	StringViewUTF8 StringUTF8TextContainer::GetString() const
	{		
		return string;
	}
	StringUTF8 StringUTF8TextContainer::SwapStrings(StringUTF8&& newString)
	{				
		StringUTF8 oldString = std::move(string);

		textUpdatedEventDispatcher.Call({
			.type = TextUpdatedEvent::Type::Erased,
			.begin = 0,
			.count = string.CharacterCount()
			});

		string = std::move(newString);		

		textUpdatedEventDispatcher.Call({
			.type = TextUpdatedEvent::Type::Inserted,
			.begin = 0,
			.count = string.CharacterCount()
			});

		return oldString;
	}
	void StringUTF8TextContainer::SetString(StringUTF8 newString)
	{
		SwapStrings(std::move(newString));		
	}

	uintMem StringUTF8TextContainer::CharacterCount() const
	{
		return string.CharacterCount();
	}
	StringUTF8 StringUTF8TextContainer::GetSubString(uintMem begin, uintMem count) const
	{
		return string.SubString(begin, count);
	}
	UnicodeChar StringUTF8TextContainer::GetCharacter(uintMem index) const
	{
		return string.SubString(index, 1).First();
	}
	void StringUTF8TextContainer::EraseSubString(uintMem begin, uintMem count)
	{
		if (count != 0)
		{
			string.EraseSubString(begin, count);

			textUpdatedEventDispatcher.Call({
				.type = TextUpdatedEvent::Type::Erased,
				.begin = begin,
				.count = count
				});
		}
	}
	void StringUTF8TextContainer::Insert(uintMem index, StringViewUTF8 string)
	{
		if (!string.Empty())
		{
			this->string.Insert(index, string);

			textUpdatedEventDispatcher.Call({
				.type = TextUpdatedEvent::Type::Inserted,
				.begin = index,
				.count = string.CharacterCount()
				});
		}
	}    
}