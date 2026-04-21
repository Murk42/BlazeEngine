#include "pch.h"
#include <BlazeEngine/UI/Text/TextLayoutMetadata.h>
#include "BlazeEngine/UI/Text/FontManager.h"
#include "BlazeEngine/UI/Text/FontFace.h"
#include <freetype/freetype.h>

namespace Blaze::UI::Text
{
	void TextLayoutMetadata::Clear()
	{
		characterData.Clear();
		lineData.Clear();
	}
	void TextLayoutMetadata::UpdateLayoutData(ArrayView<ShapedString> shapedLines, const Nodes::TextStyle& style)
	{
		UpdateLineLayout(shapedLines, style);
		UpdateGlyphLayout(shapedLines, style);
		UpdateCharacterLayout(shapedLines, style);
	}
	uintMem TextLayoutMetadata::HitTestCharacters(Vec2f pos) const
	{
		if (lineData.Empty())
			return 0;

		uintMem i = 0;
		for (; i < lineData.Count() - 1; ++i)
			if (pos.y >= lineData[i].lineStart.y)
				break;
		
		return HitTestLineCharacters(pos, i);
	}
	uintMem TextLayoutMetadata::HitTestLineCharacters(Vec2f pos, uintMem line) const
	{
		if (line >= lineData.Count())
		{
			BLAZE_LOG_FATAL("Invalid line index");
			return 0;
		}		

		uintMem begin = lineData[line].firstCharacter;
		uintMem end = line + 1 == lineData.Count() ? characterData.Count() : lineData[line + 1].firstCharacter;

		if (end == begin)
			return begin;

		Vec2f prevAdvance = { 0, 0 };
		for (uintMem i = begin; i != end; ++i)
		{
			Vec2f currAdvance = characterData[i].cursorPos;
			if ((prevAdvance.x + currAdvance.x) / 2 > pos.x)
				return i;

			prevAdvance = currAdvance;
		}

		if (line == lineData.Count() - 1 && end == characterData.Count())
			return end;
		else
			return end - 1;	
	}
	Vec2f TextLayoutMetadata::GetCursorPosBeforeCharacter(uintMem characterIndex) const
	{
		uintMem lineIndex = GetCharacterLineIndex(characterIndex);;

		Vec2f pos{ 0, 0 };

		uintMem characterLineIndex = characterIndex;

		if (lineIndex < lineData.Count())
		{
			pos += lineData[lineIndex].lineStart;

			if (lineData[lineIndex].firstCharacter > characterLineIndex)
				BLAZE_LOG_FATAL("Invalid line first character");

			characterLineIndex = characterLineIndex - lineData[lineIndex].firstCharacter;
		}

		if (characterLineIndex != 0 && characterIndex <= characterData.Count())
			pos += characterData[characterIndex - 1].cursorPos;

		return pos;
	}
	Vec2f TextLayoutMetadata::GetCursorPosAfterCharacter(uintMem characterIndex) const
	{
		uintMem lineIndex = GetCharacterLineIndex(characterIndex);

		Vec2f pos{ 0, 0 };

		uintMem characterLineIndex = characterIndex;

		if (lineIndex < lineData.Count())
		{
			pos += lineData[lineIndex].lineStart;

			if (lineData[lineIndex].firstCharacter > characterLineIndex)
				BLAZE_LOG_FATAL("Invalid line first character");

			characterLineIndex = characterLineIndex - lineData[lineIndex].firstCharacter;
		}

		if (characterIndex < characterData.Count())
			pos += characterData[characterIndex].cursorPos;

		return pos;
	}
	uintMem TextLayoutMetadata::GetCharacterLineIndex(uintMem characterIndex) const
	{
		if (characterIndex > characterData.Count())
			characterIndex = characterData.Count();

		return characterIndex == characterData.Count() ? (lineData.Empty() ? 0 : lineData.Count() - 1) : characterData[characterIndex].lineIndex;
	}
	uintMem TextLayoutMetadata::GetLineFirstCharacter(uintMem lineIndex) const
	{
		if (lineData.Empty())
			return 0;
		if (lineIndex >= lineData.Count())
			lineIndex = lineData.Count() - 1;

		return lineData[lineIndex].firstCharacter;
	}
	void TextLayoutMetadata::UpdateLineLayout(ArrayView<ShapedString> shapedLines, const Nodes::TextStyle& style)
	{
		lineData.ReserveExactly(shapedLines.Count());

		Vec2f lineAdvance{ 0, shapedLines.Count() * style.fontHeight };
		uintMem lastLineEnd = 0;

		for (auto& shapedLine : shapedLines)
		{
			lineAdvance.x = 0;
			lineAdvance.y -= style.fontHeight;

			uintMem lineBegin = lastLineEnd;
			uintMem lineEnd = shapedLine.glyphs.Empty() ? lineBegin : shapedLine.glyphs.Last().textEnd;

			lineData.AddBack(LineData{
				.firstCharacter = lineBegin,
				.lineStart = lineAdvance,
				});

			lastLineEnd = lineEnd;
		}
	}
	void TextLayoutMetadata::UpdateCharacterLayout(ArrayView<ShapedString> shapedLines, const Nodes::TextStyle& style)
	{
		characterData.Clear();

		uintMem globalGlyphIndex = 0;
		Vec2f characterAdvance;
		for (uintMem lineIndex = 0; lineIndex < shapedLines.Count(); ++lineIndex)
		{
			const Text::ShapedString& shapedLine = shapedLines[lineIndex];

			characterAdvance = { 0, 0 };

			for (uintMem glyphIndex = 0; glyphIndex < shapedLine.glyphs.Count(); ++glyphIndex, ++globalGlyphIndex)
			{				
				while (characterData.Count() <= shapedLine.glyphs[glyphIndex].textBegin)
				{
					characterData.AddBack(CharacterData{
						.lineIndex = lineIndex,
						.cursorPos = Math::Floor(characterAdvance),
						.topRight = { FLT_MIN, FLT_MIN },
						.bottomLeft = { FLT_MAX, FLT_MAX }
						});
				}

				characterAdvance += shapedLine.glyphs[glyphIndex].advance;

				characterData.Last().cursorPos = characterAdvance;

				Vec2f& topRight = characterData.Last().topRight;
				Vec2f& bottomLeft = characterData.Last().bottomLeft;

				topRight = {
					std::max(topRight.x, glyphData[globalGlyphIndex].offset.x + glyphData[globalGlyphIndex].size.x),
					std::max(topRight.y, glyphData[globalGlyphIndex].offset.y + glyphData[globalGlyphIndex].size.y)
				};
				bottomLeft = {
					std::min(bottomLeft.x, glyphData[globalGlyphIndex].offset.x),
					std::min(bottomLeft.y, glyphData[globalGlyphIndex].offset.y)
				};
			}
		}
	}
	void TextLayoutMetadata::UpdateGlyphLayout(ArrayView<ShapedString> shapedLines, const Nodes::TextStyle& style)
	{	
		uintMem totalGlyphCount = 0;
		for (auto& shapedLine : shapedLines)
			totalGlyphCount += shapedLine.glyphs.Count();

		glyphData.ReserveExactly(totalGlyphCount);

		if (style.fontManager == nullptr)
			return;

		FontManager::FontAtlasData atlasData;
		if (!style.fontManager->GetFontAtlas(style.fontName, style.fontHeight, atlasData))
		{
			BLAZE_LOG_WARNING("Couldn't find font atlas with font name: \"{}\"", style.fontName);
			return;
		}

		FT_Face fontFaceHandle = (FT_Face)atlasData.fontFace->GetHandle();
		FT_Set_Pixel_Sizes(fontFaceHandle, static_cast<FT_UInt>(style.fontHeight), 0);

		for (uintMem i = 0; i < shapedLines.Count(); ++i)
		{
			LineData line = i < lineData.Count() ? lineData[i] : LineData{ };
			Vec2f advance = { 0, 0 };
			for (auto& glyph : shapedLines[i].glyphs)
			{
				FT_Load_Glyph(fontFaceHandle, glyph.glyphIndex, FT_LOAD_COMPUTE_METRICS | FT_LOAD_NO_BITMAP);

				Vec2f offset = line.lineStart + advance + glyph.offset + Vec2f{
						static_cast<float>(fontFaceHandle->glyph->metrics.horiBearingX),
						static_cast<float>(fontFaceHandle->glyph->metrics.horiBearingY - fontFaceHandle->glyph->metrics.height)
				} / 64;
				Vec2f size = Vec2f{
					static_cast<float>(fontFaceHandle->glyph->metrics.width),
					static_cast<float>(fontFaceHandle->glyph->metrics.height)
				} / 64;

				offset = Vec2f(Math::Floor(offset.x), Math::Floor(offset.y));
				size = Vec2f(Math::Floor(size.x), Math::Floor(size.y));

				glyphData.AddBack(GlyphData{
					.characterIndex = glyph.textBegin,
					.offset = offset, 
					.size = size
					});

				advance += glyph.advance;
			}
		}
	}
}
