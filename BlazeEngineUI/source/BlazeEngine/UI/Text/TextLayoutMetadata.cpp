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
		
		uintMem begin = GetLineData(line).firstCharacter;
		uintMem end = GetLineData(line + 1).firstCharacter;

		if (end == begin)
			return begin;

		Vec2f prevAdvance = { 0, 0 };
		for (uintMem i = begin; i != end; ++i)
		{
			CharacterLayoutData characterLayoutData = GetCharacterData(i);

			Vec2f currAdvance = characterLayoutData.cursorPos;
			if ((prevAdvance.x + currAdvance.x) / 2 > pos.x)
				return i;

			prevAdvance = currAdvance;
		}

		if (line == lineData.Count() - 1 && end == characterData.Count())
			return end;
		else
			return end - 1;	
	}
	TextLayoutMetadata::LineLayoutData TextLayoutMetadata::GetLineData(uintMem lineIndex)  const
	{
		if (lineIndex >= lineData.Count())
			return LineLayoutData{
			.firstCharacter = characterData.Count() + 1, //return the + 1 character because the ending character ('\0') can be interpreted as a new line character. This case is specifically used in GetCursorPosBeforeCharacter
			.lineStart = Vec2f(0, 0),
			.width = 0.0f
			};

		return lineData[lineIndex];
	}
	TextLayoutMetadata::CharacterLayoutData TextLayoutMetadata::GetCharacterData(uintMem characterIndex) const
	{
		if (characterIndex > characterData.Count() || characterData.Empty())
			return CharacterLayoutData{
			.lineIndex = lineData.Count(),
			.cursorPos = Vec2f(),
			};

		if (characterIndex == characterData.Count())
			return characterData.Last();

		return characterData[characterIndex];
	}	
	TextLayoutMetadata::GlyphLayoutData TextLayoutMetadata::GetGlyphData(uintMem glyphIndex) const
	{
		if (glyphIndex > glyphData.Count())
			return GlyphLayoutData{
				.characterIndex = characterData.Count(),
				.glyphIndex = 0,
				.offset = { 0, 0},
				.size = { 0,0}
			};

		return glyphData[glyphIndex];
	}
	Vec2f TextLayoutMetadata::GetCursorPosBeforeCharacter(uintMem characterIndex) const
	{		
		CharacterLayoutData data = GetCharacterData(characterIndex);
		LineLayoutData lineData = GetLineData(data.lineIndex);

		if (characterIndex == lineData.firstCharacter)
			return lineData.lineStart;

		return GetCursorPosAfterCharacter(characterIndex - 1);
	}
	Vec2f TextLayoutMetadata::GetCursorPosAfterCharacter(uintMem characterIndex) const
	{
		CharacterLayoutData data = GetCharacterData(characterIndex);
		LineLayoutData lineData = GetLineData(data.lineIndex);

		return lineData.lineStart + data.cursorPos;
	}
	void TextLayoutMetadata::UpdateLineLayout(ArrayView<ShapedString> shapedLines, const Nodes::TextStyle& style)
	{
		lineData.ReserveExactly(shapedLines.Count());

		Vec2f lineAdvance{ 0, static_cast<float>(style.fontHeight) * shapedLines.Count()};
		uintMem lastLineEnd = 0;

		for (auto& shapedLine : shapedLines)
		{
			lineAdvance.x = 0;
			lineAdvance.y -= style.fontHeight;

			uintMem lineBegin = lastLineEnd;
			uintMem lineEnd = shapedLine.glyphs.Empty() ? lineBegin : shapedLine.glyphs.Last().textEnd;

			lineData.AddBack(LineLayoutData{
				.firstCharacter = lineBegin,
				.lineStart = lineAdvance,
				.width = shapedLine.extent
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
				while (characterData.Count() < shapedLine.glyphs[glyphIndex].textEnd)
				{
					characterData.AddBack(CharacterLayoutData{
						.lineIndex = lineIndex,
						.cursorPos = Math::Floor(characterAdvance)
						});
				}

				characterAdvance += shapedLine.glyphs[glyphIndex].advance;

				characterData.Last().cursorPos = characterAdvance;
			}
		}
	}
	void TextLayoutMetadata::UpdateGlyphLayout(ArrayView<ShapedString> shapedLines, const Nodes::TextStyle& style)
	{	
		uintMem totalGlyphCount = 0;
		for (auto& shapedLine : shapedLines)
			totalGlyphCount += shapedLine.glyphs.Count();

		glyphData.ReserveExactly(totalGlyphCount);

		FontManager::FontData fontData = style.GetFontData();
		
		if (fontData.fontFace == nullptr)
		{
			BLAZE_LOG_WARNING("Couldn't find font atlas with font name: \"{}\"", style.fontName);
			return;
		}

		FT_Face fontFaceHandle = (FT_Face)fontData.fontFace->GetHandle();
		FT_Set_Pixel_Sizes(fontFaceHandle, static_cast<FT_UInt>(style.fontHeight), 0);

		for (uintMem i = 0; i < shapedLines.Count(); ++i)
		{
			LineLayoutData line = i < lineData.Count() ? lineData[i] : LineLayoutData{ };
			Vec2f advance = { 0, 0 };
			for (auto& glyph : shapedLines[i].glyphs)
			{
				FT_Load_Glyph(fontFaceHandle, glyph.glyphIndex, FT_LOAD_COMPUTE_METRICS | FT_LOAD_NO_BITMAP);

				Vec2f offset = advance + glyph.offset + Vec2f{
						static_cast<float>(fontFaceHandle->glyph->metrics.horiBearingX),
						static_cast<float>(fontFaceHandle->glyph->metrics.horiBearingY - fontFaceHandle->glyph->metrics.height)
				} / 64;
				Vec2f size = Vec2f{
					static_cast<float>(fontFaceHandle->glyph->metrics.width),
					static_cast<float>(fontFaceHandle->glyph->metrics.height)
				} / 64;

				offset = Vec2f(Math::Floor(offset.x), Math::Floor(offset.y));
				size = Vec2f(Math::Floor(size.x), Math::Floor(size.y));

				glyphData.AddBack(GlyphLayoutData{
					.characterIndex = glyph.textBegin,
					.glyphIndex = glyph.glyphIndex,
					.offset = offset, 
					.size = size
					});

				advance += glyph.advance;
			}
		}
	}
}
