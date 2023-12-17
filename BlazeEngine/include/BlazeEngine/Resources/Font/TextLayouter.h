#pragma once
#include "BlazeEngine/Resources/Font/Font.h"

namespace Blaze
{
	class BLAZE_API TextLayouterBase
	{
	public:		
		struct BLAZE_API CharacterData
		{
			Vec2i pos;
			Vec2u size;
			UnicodeChar character;

			bool IsVisible() const;
		};		

		virtual ~TextLayouterBase();

		//The data returned contains exact number of elements as the string passed to SetText
		inline ArrayView<CharacterData> GetCharactersData() const { return charactersData; }
		//Width is the most right point of all the characters, height is the font height
		inline Vec2u GetSize() const { return size; }

		virtual void SetText(StringViewUTF8 text, const FontMetrics& fontMetrics) = 0;
	protected:
		TextLayouterBase() = default;

		Array<CharacterData> charactersData;
		Vec2u size;
	};

	class BLAZE_API SingleLineTextLayouter : public TextLayouterBase
	{
		uint index;
	public:
		SingleLineTextLayouter();
		SingleLineTextLayouter(StringViewUTF8 text, const FontMetrics& fontMetrics);

		void SetText(StringViewUTF8 text, const FontMetrics& fontMetrics) override;
	};	
}