#pragma once
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngineGraphics/Text/Core/TextContainerBase.h"

namespace Blaze
{
	class StringUTF8TextContainer : public TextContainerBase
	{
	public:
		StringUTF8TextContainer();
		StringUTF8TextContainer(u8String string);

		u8StringView GetString() const;
		u8String SwapStrings(u8String&& other);
		void SetString(u8String newString);

		uintMem CharacterCount() const override;		
		u8String GetSubString(uintMem begin, uintMem count) const override;
		UnicodeChar GetCharacter(uintMem index) const override;
		void EraseSubString(uintMem begin, uintMem count) override;
		void Insert(uintMem index, u8StringView string) override;
	private:
		u8String string;
	};
}