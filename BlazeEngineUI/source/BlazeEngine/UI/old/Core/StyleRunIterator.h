#pragma once
#include "BlazeEngineCore/Resource/ResourceRef.h"
#include "BlazeEngine/Resources/Font/FontFace.h"
#include "BlazeEngineGraphics/Text/Core/TextStyleData.h"

namespace Blaze
{
	struct StyleRunData
	{
		uintMem startIndex;
		uintMem endIndex;

		ResourceRef<FontFace> fontFace;
		float fontSize;
	};

	class StyleRunIterator
	{
	public: 
		StyleRunIterator(const TextStyleData* textStyleData, uintMem textStartIndex, uintMem textEndIndex, ResourceRef<TextStyle> defaultTextStyle);

		bool IsValid() const;

		StyleRunIterator& operator++();
		const StyleRunData* operator->() const;
		const StyleRunData& operator*() const;
	private:
		StyleRunData runData;
		ArrayView<TextStyleSpan>::Iterator spanIterator;
		ArrayView<TextStyleSpan>::Iterator endSpanIterator;
		const uintMem textEndIndex;
		ResourceRef<TextStyle> defaultTextStyle;

		static bool CanTextStylesBeCombined(const TextStyle& a, const TextStyle& b);
		static bool CanTextStylesBeCombined(const StyleRunData& runData, const TextStyle& style);
		static void CopyStyleToRun(StyleRunData& runData, const TextStyle& style);

		void FindNextStyleRun();
		void TryExpandingRun();
	};
}