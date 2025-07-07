#pragma once
#include "BlazeEngineCore/Event/EventDispatcher.h"
#include "BlazeEngineCore/DataStructures/StringUTF8.h"
#include "BlazeEngineCore/DataStructures/StringViewUTF8.h"

namespace Blaze
{
	class TextContainerBase
	{
	public:
		struct TextUpdatedEvent
		{
			enum class Type
			{
				Erased, Inserted
			};

			Type type;
			uintMem begin;
			uintMem count;
		};
		EventDispatcher<TextUpdatedEvent> textUpdatedEventDispatcher;

		virtual uintMem CharacterCount() const = 0;
		virtual StringUTF8 GetSubString(uintMem begin, uintMem count) const = 0;
		virtual UnicodeChar GetCharacter(uintMem index) const = 0;
		virtual void EraseSubString(uintMem begin, uintMem count) = 0;
		virtual void Insert(uintMem index, StringViewUTF8 string) = 0;
	};
}