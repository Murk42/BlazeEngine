#pragma once
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngineGraphics/Text/Core/TextContainerBase.h"

namespace Blaze
{
	class TextContainerReference : public TextContainerBase
	{
	public:		
		TextContainerReference(TextContainerBase& container);
		~TextContainerReference();

		void SetContainer(TextContainerBase& newContainer);

		inline uintMem CharacterCount() const override final { if (container != nullptr) return container->CharacterCount(); else return 0; }
		inline u8String GetSubString(uintMem begin, uintMem count) const override final { return container->GetSubString(begin, count); }
		inline UnicodeChar GetCharacter(uintMem index) const override final { return container->GetCharacter(index); }
		inline void EraseSubString(uintMem begin, uintMem count) override final { container->EraseSubString(begin, count); }
		inline void Insert(uintMem index, u8StringView string) override final { container->Insert(index, string); }
	private:
		TextContainerBase* container;

		void TextUpdatedEvent(const TextContainerBase::TextUpdatedEvent& event);
	};
}