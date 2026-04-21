#pragma once

#include "BlazeEngine/Core/String/String.h"
#include "BlazeEngine/Core/Container/Array.h"

namespace Blaze::UI
{
	class TextSeparationDataBase
	{
	public:
		virtual uint32 GetSeparationValue(uintMem index) const = 0;
		virtual uint32 GetMaximumWrappingSeparationValue() const = 0;
		virtual bool IsNewLine(uintMem index) const = 0;
		virtual uintMem Count() const = 0;
	};

	class DefaultTextSeparationData : public TextSeparationDataBase
	{
	public:
		enum TextSeparation : uint32
		{
			Codepoint = 0,
			Grapheme = 1,
			WordSuffix = 2,
			Word = 3,
			NewLine = 4
		};

		DefaultTextSeparationData() = default;
		DefaultTextSeparationData(u8StringView text);
		DefaultTextSeparationData(DefaultTextSeparationData&& other) noexcept = default;

		void SetText(u8StringView);
			
		uint32 GetSeparationValue(uintMem index) const override;
		uint32 GetMaximumWrappingSeparationValue() const override;
		bool IsNewLine(uintMem index) const override;
		uintMem Count() const override;

		DefaultTextSeparationData& operator=(DefaultTextSeparationData&& other) noexcept = default;
	protected:
		virtual bool IsAdditionalWordCharacter(UnicodeChar character);
		virtual bool IsWordSuffixCharacter(UnicodeChar unicodeChar);
	private:	
		Array<TextSeparation> separations;
	};
}