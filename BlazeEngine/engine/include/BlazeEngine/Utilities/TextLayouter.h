#pragma once
#include "BlazeEngine/Resources/Font/Font.h"

namespace Blaze
{
	class BLAZE_API TextLayouterBase
	{
	public:		
		struct BLAZE_API CharacterData
		{
			Vec2f pos;
			Vec2f size;
			UnicodeChar character;

			bool IsVisible() const;
		};		

		inline ArrayView<CharacterData> GetCharactersData() const { return charactersData; }
		inline Vec2f GetSize() const { return size; }
	protected:
		TextLayouterBase() = default;

		Array<CharacterData> charactersData;
		Vec2f size;
	};

	class BLAZE_API SingleLineTextLayouter : public TextLayouterBase
	{
		uint index;
	public:
		SingleLineTextLayouter();
		SingleLineTextLayouter(StringViewUTF8 text, const Font& font, const FontResolution& fontResolution);		

		void Setup(StringViewUTF8 text, const Font& font, const FontResolution&fontResolution);
	};

	class MultiLineTextLayouter : public TextLayouterBase
	{		
		Array<CharacterData> charactersData;
		uint index;
	public:
		MultiLineTextLayouter(StringViewUTF8 text, FontResolution* fontResolution);
	};
}