#pragma once
#include "BlazeEngine/UI/Text/TextShaping/TextShaping.h"
#include "BlazeEngine/UI/Text/TextSeparations.h"

namespace Blaze::UI::TextShaping
{
	class WrappedTextShapingContext : public ShapingContext
	{
	public:
		using ShouldIncludeAtEndFunctionType = bool(*)(UnicodeChar character, const WrappedTextShapingContext& shapingContext);

		WrappedTextShapingContext(u8StringView text, const FontFace& fontFace, uint32 fontHeight, float maxWidth, const TextSeparationDataBase& textSeparation, ShouldIncludeAtEndFunctionType shouldIncludeAtEndFunction = ShouldIncludeAtEndDefault);

		bool ShouldIncludeAtEnd(UnicodeChar character) const { return shouldIncludeAtEndFunction(character, *this); }
		inline const TextSeparationDataBase& GetTextSeparationData() const { return textSeparationData; }
		inline float GetMaxWidth() const { return maxWidth; }

		static bool ShouldIncludeAtEndDefault(UnicodeChar character, const WrappedTextShapingContext& shapingContext);
	private:
		const TextSeparationDataBase& textSeparationData;
		float maxWidth;

		//function which will choose if a non-null width character should be included at the end of a line
		bool(*shouldIncludeAtEndFunction)(UnicodeChar character, const WrappedTextShapingContext& shapingContext) = ShouldIncludeAtEndDefault;
	};
	
	BLAZE_API Array<ShapingResult> ShapeTextWrapped(u8StringView text, const WrappedTextShapingContext& shapingContext);
}