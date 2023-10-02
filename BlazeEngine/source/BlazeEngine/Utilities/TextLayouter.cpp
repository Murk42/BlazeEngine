#include "BlazeEngine/Utilities/TextLayouter.h"

namespace Blaze
{
	bool TextLayouterBase::CharacterData::IsVisible() const
	{
		return size.x != 0 && size.y != 0;
	}
	SingleLineTextLayouter::SingleLineTextLayouter()
		: index(0)
	{
	}
	SingleLineTextLayouter::SingleLineTextLayouter(StringViewUTF8 text, const Font& font, const FontResolution& fontResolution)
	{		
		Setup(text, font, fontResolution);
	}
	void SingleLineTextLayouter::Setup(StringViewUTF8 text, const Font& font, const FontResolution& fontResolution)
	{
		charactersData.Resize(text.CharacterCount());

		float cursor = 0.0f;

		bool isFirst = true;
		bool isLast = false;

		UnicodeChar prevCharacter;

		auto it = charactersData.FirstIterator();
		for (auto character : text)
		{
			if (it == charactersData.LastIterator())
				isLast = true;

			FontResolutionGlyphData characterMetrics;
			fontResolution.GetGlyphData(character, characterMetrics);

			it->pos = Vec2f(cursor, 0) + characterMetrics.offset;
			it->size = characterMetrics.size;
			it->character = character;

			if (!isFirst)
				it->pos += font.GetKerning(fontResolution, prevCharacter, character);

			if (!isLast)
				cursor += characterMetrics.horizontalAdvance;

			prevCharacter = character;

			++it;
		}

		size.x = cursor + charactersData.Last().size.x;
		size.y = fontResolution.GetFontHeight();
	}
}
