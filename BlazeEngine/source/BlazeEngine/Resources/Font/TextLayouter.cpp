#include "pch.h"
#include "BlazeEngine/Resources/Font/TextLayouter.h"

namespace Blaze
{
	Blaze::TextLayouterBase::~TextLayouterBase()
	{
	}
	bool TextLayouterBase::CharacterData::IsVisible() const
	{
		return size.x != 0 && size.y != 0;
	}
	SingleLineTextLayouter::SingleLineTextLayouter()
		: index(0)
	{
	}
	SingleLineTextLayouter::SingleLineTextLayouter(StringViewUTF8 text, const FontMetrics& fontMetrics)
	{		
		SetText(text, fontMetrics);
	}
	void SingleLineTextLayouter::SetText(StringViewUTF8 text, const FontMetrics& fontMetrics)
	{
		charactersData.Resize(text.CharacterCount());

		uint cursor = 0;

		bool isFirst = true;
		bool isLast = false;

		UnicodeChar prevCharacter;

		auto it = charactersData.FirstIterator();
		for (auto character : text)
		{
			if (it == charactersData.LastIterator())
				isLast = true;

			FontGlyphMetrics characterMetrics;
			fontMetrics.GetGlyphMetrics(character, characterMetrics);

			it->pos = Vec2i(cursor, 0) + characterMetrics.offset;
			it->size = characterMetrics.size;
			it->character = character;

			if (!isFirst)
				it->pos += fontMetrics.GetGlyphKerning(prevCharacter, character);

			if (!isLast)
				cursor += characterMetrics.horizontalAdvance;

			prevCharacter = character;

			++it;
		}

		size.x = cursor + charactersData.Last().size.x;
		size.y = fontMetrics.GetPixelFontHeight();
	}
}
