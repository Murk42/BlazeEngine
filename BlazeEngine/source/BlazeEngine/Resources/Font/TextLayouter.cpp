#include "pch.h"
#include "BlazeEngine/Resources/Font/TextLayouter.h"

namespace Blaze
{
	float FillCharacterLayoutData(CharacterLayoutData& data, float cursor, const FontGlyphMetrics& metrics, const FontMetrics& fontMetrics, UnicodeChar character, UnicodeChar prevCharacter)
	{
		data.pos = Vec2f(cursor, 0) + metrics.offset;
		data.size = metrics.size;
		data.character = character;

		if (data.size.x == 0)
			data.size.x = metrics.horizontalAdvance;

		Vec2f kerning;
		if (prevCharacter != '\0')
			kerning = fontMetrics.GetGlyphKerning(prevCharacter, character);

		data.pos += kerning;
					
		return metrics.horizontalAdvance + kerning.x;
	}

	SingleLineTextLayouter::SingleLineTextLayouter()		
	{
	}
	SingleLineTextLayouter::SingleLineTextLayouter(StringViewUTF8 text, const FontMetrics& fontMetrics)
	{		
		SetText(text, fontMetrics);
	}
	void SingleLineTextLayouter::SetText(StringViewUTF8 text, const FontMetrics& fontMetrics)
	{
		lines.Clear();

		if (text.Empty())					
			return;		

		float cursor = 0;
		float baselineDistance = fontMetrics.GetPixelFontHeight();		

		auto line = &*lines.AddBack();
		line->characters.Resize(text.CharacterCount());

		UnicodeChar prevCharacter = '\0';

		float lastAdvance = 0.0f;
		float lastWidth = 0.0f;
		auto textIt = text.FirstIterator();
		for (uintMem i = 0; i < text.CharacterCount(); ++i, ++textIt)
		{
			UnicodeChar character = *textIt;
			auto& characterData = line->characters[i];

			FontGlyphMetrics metrics;
			if (!fontMetrics.GetGlyphMetrics(character, metrics))
			{
				//Create empty character				
				characterData.character = character;
				characterData.pos = Vec2f(cursor + lastAdvance, 0);
				characterData.size = Vec2f(0, 0);

				continue;
			}

			lastAdvance = FillCharacterLayoutData(characterData, cursor, metrics, fontMetrics, character, prevCharacter);
			lastWidth = characterData.size.x;

			cursor += lastAdvance;
				
			prevCharacter = character;			
		}

		line->width = cursor - lastAdvance + lastWidth;
	}
	MultiLineTextLayouter::MultiLineTextLayouter()		
	{ 
	}
	MultiLineTextLayouter::MultiLineTextLayouter(StringViewUTF8 text, const FontMetrics& fontMetrics)
	{
		SetText(text, fontMetrics);
	}
	void MultiLineTextLayouter::SetText(StringViewUTF8 text, const FontMetrics& fontMetrics)
	{		
		lines.Clear();

		if (text.Empty())		
			return;				

		float cursor = 0.0f;
		
		UnicodeChar prevCharacter = '\0';

		auto* line = &*lines.AddBack();

		float lastAdvance = 0.0f;
		float lastWidth = 0.0f;
		auto textIt = text.FirstIterator();
		for (uintMem i = 0; i < text.CharacterCount(); ++i, ++textIt)
		{
			UnicodeChar character = *textIt;

			if (character.Value() == (uint32)'\n')
			{				
				//Create empty character
				auto& characterData = *line->characters.AddBack();
				characterData.character = character;
				characterData.pos = Vec2f(cursor + lastAdvance, 0);
				characterData.size = Vec2f(0, 0);

				line->width = cursor - lastAdvance + lastWidth;
				cursor = 0;				
				line = &*lines.AddBack();						
				continue;
			}

			FontGlyphMetrics metrics;
			if (!fontMetrics.GetGlyphMetrics(character, metrics))
			{
				//Create empty character
				auto& characterData = *line->characters.AddBack();
				characterData.character = character;
				characterData.pos = Vec2f(cursor + lastAdvance, 0);
				characterData.size = Vec2f(0, 0);

				continue;
			}
			
			auto& characterData = *line->characters.AddBack();
			lastAdvance = FillCharacterLayoutData(characterData, cursor, metrics, fontMetrics, character, prevCharacter);
			lastWidth = characterData.size.x;

			cursor += lastAdvance;

			prevCharacter = character;
		}		
		
		line->width = cursor - lastAdvance + lastWidth;
	}
	WrappedLineTextLayouter::WrappedLineTextLayouter()
		: wrapWidth(0)
	{
	}
	WrappedLineTextLayouter::WrappedLineTextLayouter(StringViewUTF8 text, const FontMetrics& fontMetrics, float wrapWidth)
		: wrapWidth(wrapWidth)
	{
	}
	void WrappedLineTextLayouter::SetWrapWidth(float wrapWidth)
	{
		this->wrapWidth = wrapWidth;
	}
	void WrappedLineTextLayouter::SetText(StringViewUTF8 text, const FontMetrics& fontMetrics)
	{
		lines.Clear();

		if (text.Empty())
			return;


		float cursor = 0;		

		auto* line = &*lines.AddBack();		

		UnicodeChar prevCharacter = '\0';

		float lastAdvance = 0.0f;
		float lastWidth = 0.0f;
		auto textIt = text.FirstIterator();
		for (uintMem i = 0; i < text.CharacterCount(); ++i, ++textIt)
		{
			UnicodeChar character = *textIt;

			if (character.Value() == (uint32)'\n')
			{
				//Create empty character
				auto& characterData = *line->characters.AddBack();
				characterData.character = character;
				characterData.pos = Vec2f(cursor + lastAdvance, 0);
				characterData.size = Vec2f(0, 0);

				line->width = cursor - lastAdvance + lastWidth;
				cursor = 0;
				line = &*lines.AddBack();
				continue;
			}

			FontGlyphMetrics metrics;
			if (!fontMetrics.GetGlyphMetrics(character, metrics))
			{
				//Create empty character
				auto& characterData = *line->characters.AddBack();
				characterData.character = character;
				characterData.pos = Vec2f(cursor + lastAdvance, 0);
				characterData.size = Vec2f(0, 0);

				continue;
			}

			if (cursor + metrics.size.x > wrapWidth && wrapWidth != 0 && metrics.size.x <= wrapWidth)
			{
				line->width = cursor - lastAdvance + lastWidth;
				cursor = 0;
				line = &*lines.AddBack();
			}

			auto& characterData = *line->characters.AddBack();

			lastAdvance = FillCharacterLayoutData(characterData, cursor, metrics, fontMetrics, character, prevCharacter);
			lastWidth = characterData.size.x;

			cursor += lastAdvance;

			prevCharacter = character;
		}

		line->width = cursor - lastAdvance + lastWidth;
	}
}
