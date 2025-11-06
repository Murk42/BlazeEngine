#pragma once
#include "BlazeEngine/UI/Text/TextShaping/TextShaping.h"

namespace Blaze::UI::TextShaping
{
	enum TextUnitSeparationLevel
	{
		None = 0,
		Codepoint = 1,
		Grapheme = 2,
		Word = 3,
		NewLine = 4,
	};

	class WrappedTextShapingContext : public ShapingContext
	{
	public:
		using ShouldIncludeAtEndFunctionType = bool(*)(UnicodeChar character, const WrappedTextShapingContext& shapingContext);

		WrappedTextShapingContext(u32StringView text, const FontFace& fontFace, uint32 maxWidth,
			TextUnitSeparationLevel smallestSeparationLevel = TextUnitSeparationLevel::Grapheme,
			TextUnitSeparationLevel expectedSeparationLevel = TextUnitSeparationLevel::Word,
			ShouldIncludeAtEndFunctionType shouldIncludeAtEndFunction = ShouldIncludeAtEndDefault);

		bool ShouldIncludeAtEnd(UnicodeChar character) const { return shouldIncludeAtEndFunction(character, *this); }

		TextUnitSeparationLevel GetTextUnitSeparationAt(u32StringViewIterator iterator) const;

		inline TextUnitSeparationLevel GetExpectedSeparationLevel() const { return expectedSeparationLevel; }
		inline TextUnitSeparationLevel GetSmallestSeparationLevel() const { return smallestSeparationLevel; }
		inline uint32 GetMaxWidth() const { return maxWidth; }

		static bool ShouldIncludeAtEndDefault(UnicodeChar character, const WrappedTextShapingContext& shapingContext);
	private:
		Array<TextUnitSeparationLevel> textUnitSeparations;
		TextUnitSeparationLevel smallestSeparationLevel = TextUnitSeparationLevel::Grapheme;
		TextUnitSeparationLevel expectedSeparationLevel = TextUnitSeparationLevel::Word;
		uint32 maxWidth;

		//function which will choose if a non-null width character should be included at the end of a line
		bool(*shouldIncludeAtEndFunction)(UnicodeChar character, const WrappedTextShapingContext& shapingContext) = ShouldIncludeAtEndDefault;
	};


	BLAZE_API Array<ShapedText> ShapeTextWrapped(u32StringViewIterator textBegin, u32StringViewIterator textEnd, const WrappedTextShapingContext& shapingContext);
}