#pragma once
#include "BlazeEngineCore/Event/EventDispatcher.h"
#include "BlazeEngineCore/String/String.h"

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
			TextContainerBase& textContainer;
		};
		EventDispatcher<TextUpdatedEvent> textUpdatedEventDispatcher;

		virtual uintMem CharacterCount() const = 0;
		virtual u8String GetSubString(uintMem begin, uintMem count) const = 0;
		virtual UnicodeChar GetCharacter(uintMem index) const = 0;
		virtual void EraseSubString(uintMem begin, uintMem count) = 0;
		virtual void Insert(uintMem index, u8StringView string) = 0;
	};
}