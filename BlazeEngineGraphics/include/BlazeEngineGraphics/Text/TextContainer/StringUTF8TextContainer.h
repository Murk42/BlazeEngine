#pragma once
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngineGraphics/Text/Core/TextContainerBase.h"

namespace Blaze
{
	class StringUTF8TextContainer : public TextContainerBase
	{
	public:
		StringUTF8TextContainer();
		StringUTF8TextContainer(StringUTF8 string);

		StringViewUTF8 GetString() const;
		StringUTF8 SwapStrings(StringUTF8&& other);
		void SetString(StringUTF8 newString);

		uintMem CharacterCount() const override;		
		StringUTF8 GetSubString(uintMem begin, uintMem count) const override;
		UnicodeChar GetCharacter(uintMem index) const override;
		void EraseSubString(uintMem begin, uintMem count) override;
		void Insert(uintMem index, StringViewUTF8 string) override;
	private:
		StringUTF8 string;
	};
}