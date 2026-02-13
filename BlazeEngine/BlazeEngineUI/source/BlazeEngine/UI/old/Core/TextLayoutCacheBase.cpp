#include "pch.h"
#include "BlazeEngineGraphics/Text/Core/TextLayoutCacheBase.h"

#include "BlazeEngineGraphics/Text/Core/StyleRunIterator.h"

#include <harfbuzz/hb-ft.h>
#include <harfbuzz/hb.h>
#include <linebreak.h>

#include "BlazeEngine/Console/Console.h"

#include "BlazeEngineCore/DataStructures/RawArray.h"

namespace Blaze
{
	namespace
	{
		/*
		bool CheckStyleSpans(ArrayView<TextStyleSpan> styleSpans)
		{
			if (styleSpans.Empty())
				return true;

			FontFaceDirection textDirection = styleSpans[0].style->GetFontFace()->GetDirection();

			uintMem previousStyleSpanEnd = 0;
			for (auto& styleSpan : styleSpans)
			{
				if (styleSpan.style.Get().GetFontFace()->GetDirection() != textDirection)
				{
					BLAZE_ENGINE_GRAPHICS_ERROR("Not all font faces in the font style spans are the same direction");
					return false;
				}

				if (styleSpan.startIndex < previousStyleSpanEnd)
				{
					BLAZE_ENGINE_GRAPHICS_ERROR("Style spans overlap");
					styleSpans = { };
					break;
				}

				previousStyleSpanEnd = styleSpan.endIndex;
			}

			return true;
		}

		enum class LineBreakAttribute : uint8
		{
			AllowBreak,
			NoBreak,
			MustBreak
		};
		Array<LineBreakAttribute> GetLineBreakingAttributes(const  StringViewUTF8& text, const char* language)
		{
			Array<LineBreakAttribute> lineBreakAttributes(text.CharacterCount());
			static struct LibunibreakInit {
				LibunibreakInit()
				{
					init_linebreak();
				}
			} libunibreakInit;
			set_linebreaks_utf8_per_code_point((const utf8_t*)text.Buffer(), text.BufferSize(), language, (char*)lineBreakAttributes.Ptr());

			for (auto& value : lineBreakAttributes)
				switch ((char)value)
				{
				case LINEBREAK_INDETERMINATE:
				case LINEBREAK_ALLOWBREAK:
					value = LineBreakAttribute::AllowBreak;
					break;
				case LINEBREAK_MUSTBREAK:
					value = LineBreakAttribute::MustBreak;
					break;
				case LINEBREAK_NOBREAK:
					value = LineBreakAttribute::NoBreak;
					break;
				default:
					BLAZE_ENGINE_ERROR("Invalid line break attribute");
					break;
				}

			return lineBreakAttributes;
		}

		void ExtractGlyphData(hb_buffer_t* buffer, FT_Face face, TextLayoutCacheBase::GlyphShapingData* glyphs)
		{
			static_assert(std::is_trivially_copyable_v<TextLayoutCacheBase::GlyphShapingData>);

			uint glyphCount;
			const hb_glyph_info_t* glyphInfo = hb_buffer_get_glyph_infos(buffer, &glyphCount);
			const hb_glyph_position_t* glyphPos = hb_buffer_get_glyph_positions(buffer, &glyphCount);

			for (uintMem i = 0; i < glyphCount; ++i)
			{
				FT_Load_Glyph(face, glyphInfo[i].codepoint, FT_LOAD_NO_BITMAP);

				const Vec2i16 shapingOffset = Vec2i16(static_cast<int16>(glyphPos[i].x_offset / 64), static_cast<int16>(glyphPos[i].y_offset / 64));
				const Vec2i16 offset = Vec2i16(static_cast<int16>(face->glyph->metrics.horiBearingX / 64), static_cast<int16>((face->glyph->metrics.horiBearingY - face->glyph->metrics.height) / 64));

				//division by 64 is used because font metrics are in a 26.6 fixed-point format
				glyphs[i] = TextLayoutCacheBase::GlyphShapingData{
					.glyphIndex = glyphInfo[i].codepoint,
					.advance = Vec2i16(static_cast<int16>(glyphPos[i].x_advance / 64), static_cast<int16>(glyphPos[i].y_advance / 64)),
					.offset = offset + shapingOffset,
					.size = Vec2u16(static_cast<uint16>(face->glyph->metrics.height / 64), static_cast<uint16>(face->glyph->metrics.width / 64)),
					.cluster = glyphInfo[i].cluster
				};
			}
		}

		struct BreakPosition
		{
			uintMem textBreakPos;
			uintMem glyphBreakPos;
		};

		BreakPosition ExtractGlyphDataUntilWrap(Array<hb_glyph_position_t>& glyphPositions, FontFace& face, uintMem startPos, uintMem endPos, Vec2f bounds, Array<TextLayoutCacheBase::GlyphShapingData>& glyphs)
		{
		}
		
		Array<bool> ExtractBreakFlags(hb_buffer_t* buffer, uintMem textSize)
		{
			Array<bool> flags(textSize, true);

			uint glyphCount;
			const hb_glyph_info_t* glyphInfo = hb_buffer_get_glyph_infos(buffer, &glyphCount);
			for (uintMem i = 0; i < glyphCount; ++i)
				if (static_cast<bool>(hb_glyph_info_get_glyph_flags(glyphInfo + i) & HB_GLYPH_FLAG_UNSAFE_TO_BREAK))
					flags[glyphInfo[i].cluster] = false;

			return flags;
		}

		uintMem GetGlyphsWrapPosition(const ArrayView<TextLayoutCacheBase::GlyphShapingData>& glyphs, Vec2f bounds, FontFaceDirection direction)
		{
			if (direction == FontFaceDirection::Horizontal)
			{				
				int64 cursor = 0;
				for (uintMem i = 0; i < glyphs.Count(); ++i)
				{
					if (static_cast<int64>(bounds.x) <= cursor + static_cast<int64>(glyphs[i].offset.x) + static_cast<int64>(glyphs[i].size.x))
						return glyphs[i].cluster;

					cursor += static_cast<int64>(glyphs[i].advance.x);
				}
			}
			else
			{
				int64 cursor = 0;
				for (uintMem i = 0; i < glyphs.Count(); ++i)
				{
					if (static_cast<int64>(bounds.y) <= cursor + static_cast<int64>(glyphs[i].offset.y) + static_cast<int64>(glyphs[i].size.y))
						return glyphs[i].cluster;

					cursor += glyphs[i].advance.y;
				}
			}

			return glyphs.Count();
		}

		void ShapeText(hb_buffer_t* buffer, const StringViewUTF8& text, uintMem textStart, uintMem textEnd, hb_font_t* font)
		{
			hb_buffer_add_utf8(buffer, static_cast<const char*>(text.Buffer()), text.CharacterCount()(), textStart, textEnd - textStart);

			//TODO multi-directional text isn't supported
			hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
			hb_buffer_guess_segment_properties(buffer);

			hb_buffer_set_flags(buffer, static_cast<hb_buffer_flags_t>(
				(textStart == 0 ? HB_BUFFER_FLAG_BOT : 0) |
				(textEnd == text.CharacterCount() ? HB_BUFFER_FLAG_EOT : 0))
			);

			hb_shape(font, buffer, nullptr, 0);
		}

		void CleanGlyphsAroundBreak(const StringViewUTF8& text, uintMem textStart, uintMem textEnd, uintMem breakPos, hb_font_t* font, Array<TextLayoutCacheBase::GlyphShapingData>& glyphs, Array<GlyphMetadata>& glyphMetadata)
		{
			if (!glyphMetadata[breakPos].unsafeToBreak)
				return;

			uintMem leftRunStart = breakPos;
			while (leftRunStart != 0 && glyphMetadata[leftRunStart].unsafeToBreak)
				--leftRunStart;

			if (leftRunStart != breakPos)
			{
				Array<TextLayoutCacheBase>
				hb_buffer_t* leftRunBuffer = ShapeText(text, leftRunStart, breakPos, font);

				ExtractGlyphShapingData(leftRunBuffer, ft_face, glyphs);

				hb_buffer_destroy(leftRunBuffer);
			}

			uintMem rightRunEnd = wrapPos;
			while (rightRunEnd != glyphs.Count() && glyphMetadata[rightRunEnd].unsafeToBreak)
				++rightRunEnd;


			if (rightRunEnd != wrapPos)
			{
				glyphs.Resize(leftRunStart);

				hb_buffer_t* leftRunBuffer = ShapeText(text, leftRunStart, wrapPos, font);

				glyphs.ReserveAdditional(hb_buffer_get_length(leftRunBuffer));
				ExtractGlyphShapingData(leftRunBuffer, ft_face, glyphs);

				hb_buffer_destroy(leftRunBuffer);
			}
		}

		struct ShapeContext
		{
			StringViewUTF8 text;
			FontFace& fontFace;
			float fontSize;
		};

		hb_font_t* PrepareShapeContext(const ShapeContext& shapeContext)
		{
			const auto ft_face = static_cast<FT_Face>(shapeContext.fontFace.GetHandle());
			FT_Set_Pixel_Sizes(ft_face, static_cast<FT_UInt>(shapeContext.fontSize), 0);

			hb_font_t* font = hb_ft_font_create_referenced(ft_face);
			hb_ft_font_set_load_flags(font, FT_LOAD_NO_BITMAP);

			return font;
		}

		Array<uintMem> ShapeStyleRuns(const ShapeContext& shapeContext, uintMem textStart, uintMem textEnd, Vec2f bounds, Array<TextLayoutCacheBase::GlyphShapingData>& glyphs)
		{
			const uintMem runSize = textEnd - textStart;

			hb_font_t* font = PrepareShapeContext(shapeContext);

			hb_buffer_t* buffer = hb_buffer_create();
			ShapeText(buffer, shapeContext.text, textStart, textEnd, font);

			Array<hb_glyph_position_t> glyphPositions(hb_buffer_get_length(buffer));
			memcpy(glyphPositions.Ptr(), hb_buffer_get_glyph_positions(buffer, nullptr), glyphPositions.Count());

			Array<hb_glyph_info_t> glyphInfo(hb_buffer_get_length(buffer));
			memcpy(glyphInfo.Ptr(), hb_buffer_get_glyph_infos(buffer, nullptr), glyphInfo.Count());

			Array< TextLayoutCacheBase::GlyphShapingData> currentLineGlyphs;

			Array<uintMem> breakPositions;

			uintMem lastRunBreakPos = 0;
			auto [runBreakPos, glyphBreakPos] = ExtractGlyphDataUntilWrap(glyphPositions, shapeContext.fontFace, lastRunBreakPos, runSize, bounds, currentLineGlyphs);

			while (true)
			{
				if (runBreakPos == runSize)
					break;

				//TODO Make the breakPos be at the end of a word, instead of inside a word

				if (!breakFlags[runBreakPos])
				{
					//Get the first previous character after the last break position that is safe to break
					uintMem leftRunStart = runBreakPos - 1;
					while (leftRunStart != lastRunBreakPos && !breakFlags[leftRunStart])
						--leftRunStart;

					const uintMem validGlyphCount = runBreakPos - leftRunStart;

					hb_buffer_t* tempBuffer = hb_buffer_create();
					ShapeText(tempBuffer, text, leftRunStart, runBreakPos, font);

					const uintMem newGlyphCount = hb_buffer_get_length(tempBuffer);

					uintMem dstOffset = glyphs.Count();
					glyphs.Resize(glyphs.Count() + validGlyphCount + newGlyphCount);

					static_assert(std::is_trivially_copyable_v<TextLayoutCacheBase::GlyphShapingData>);
					memcpy(glyphs.Ptr() + dstOffset, currentLineGlyphs.Ptr(), validGlyphCount * sizeof(TextLayoutCacheBase::GlyphShapingData));

					dstOffset += validGlyphCount;
					ExtractGlyphData(buffer, ft_face, glyphs.Ptr() + dstOffset);

					currentLineGlyphs.Clear();

					uintMem rightRunStart = runBreakPos + 1;
					while (rightRunStart != runSize && !breakFlags[rightRunStart])
						++rightRunStart;

					hb_buffer_reset(tempBuffer);
					ShapeText(tempBuffer, text, runBreakPos, rightRunStart, font);
					auto [newRunBreakPos, newGlyphBreakPos] = ExtractGlyphDataUntilWrap(buffer, ft_face, runBreakPos, currentLineGlyphs);

					breakPositions.AddBack(runBreakPos + textStart);
					lastRunBreakPos = runBreakPos;

					runBreakPos = newRunBreakPos;
					glyphBreakPos = newGlyphBreakPos;

					//Shape remaining of the next line
					hb_buffer_destroy(tempBuffer);
				}
				else
				{
					glyphs.Append(std::move(currentLineGlyphs));

					breakPositions.AddBack(textBreakPos);
					lastTextBreakPos = textBreakPos;
				}
			}

			hb_buffer_destroy(buffer);

			return breakPositions;
		}

		void ShapeLines(const StringViewUTF8& text, uintMem startIndex, uintMem endIndex, const TextStyleData* textStyleData, ResourceRef<TextStyle> defaultTextStyle, Vec2f bounds, Array<TextLayoutCacheBase::GlyphShapingData>& glyphs)
		{
			StringViewUTF8Iterator end = text.FirstIterator();
			for (uintMem i = 0; i < startIndex; ++i)
				++end;

			StyleRunIterator styleRunIterator{ textStyleData, startIndex, endIndex, defaultTextStyle };
			for (; styleRunIterator.IsValid(); ++styleRunIterator)
			{
				StringViewUTF8Iterator begin = end;
				for (uintMem i = 0; i < styleRunIterator->endIndex - styleRunIterator->startIndex; ++i)
					++end;

				ShapeStyleRuns(text, startIndex, endIndex, *styleRunIterator->fontFace, styleRunIterator->fontSize, bounds, glyphs);
			}
		}
		*/


	}

	TextLayoutCacheBase::TextLayoutCacheBase()
		: textStyleData(nullptr)
	{		
	}
	TextLayoutCacheBase::~TextLayoutCacheBase()
	{
		if (this->textStyleData != nullptr)
		{
			this->textStyleData->textStyleSpanChangedEventDispathcer.RemoveHandler<&TextLayoutCacheBase::TextStyleSpanChangedEvent>(*this);
		}
	}
	void TextLayoutCacheBase::CreateCache(StringViewUTF8 text, TextStyleData* textStyleData, ResourceRef<TextStyle> defaultTextStyle)
	{
		this->text = text;

		if (this->textStyleData != nullptr)
		{
			this->textStyleData->textStyleSpanChangedEventDispathcer.RemoveHandler<&TextLayoutCacheBase::TextStyleSpanChangedEvent>(*this);
		}

		this->textStyleData = textStyleData;

		if (this->textStyleData != nullptr)
		{
			this->textStyleData->textStyleSpanChangedEventDispathcer.AddHandler<&TextLayoutCacheBase::TextStyleSpanChangedEvent>(*this);
		}

		this->defaultTextStyle = defaultTextStyle;

		CreateTextData();
	}
	void TextLayoutCacheBase::TextStyleSpanChangedEvent(const TextStyleData::TextStyleSpanChangedEvent& event)
	{
		CreateTextData();
	}
	void TextLayoutCacheBase::CreateTextData()
	{
		graphemeData.Clear();
		lineData.Clear();
		glyphData.Clear();		

		auto lineBreakingAttributes = GetLineBreakingAttributes(text, hb_language_to_string(hb_language_get_default()));
		
		Array<GlyphShapingData> glyphs;

		uintMem prevBreak = 0;
		for (uintMem i = 0; i < text.CharacterCount(); ++i)
			if (i == text.CharacterCount() - 1 || lineBreakingAttributes[i] == LineBreakAttribute::MustBreak)
			{
				ShapeText(text, prevBreak, i + 1, textStyleData, defaultTextStyle, glyphs);
				prevBreak = i + 1;
			}

		Console::Write("\n");
		Console::Write("\n");

		Console::Write("{:>10}:", "Text");
		for (auto ch : text)
		{
			if (ch == '\n')
				Console::Write("|");
			else
				Console::Write(StringViewUTF8() + ch);
		}
		Console::Write("\n");

		Console::Write("{:>10}:", "Line break");
		for (uintMem i = 0; i < text.CharacterCount(); ++i)
		{			
			if (lineBreakingAttributes[i] == LineBreakAttribute::MustBreak)
				Console::Write("|");
			if (lineBreakingAttributes[i] == LineBreakAttribute::AllowBreak)
				Console::Write(":");
			if (lineBreakingAttributes[i] == LineBreakAttribute::NoBreak)
				Console::Write(" ");
		}
		Console::Write("\n");

		Console::Write("{:>10}:", "Glyph");
		uintMem glyphIndex = 0;
		bool unsafeToBreak = false;
		bool unsafeToConcat = false;
		for (uintMem i = 0; i < text.CharacterCount(); ++i)
		{
			bool firstGlyph = true;
			while (glyphIndex != glyphs.Count() && i == glyphs[glyphIndex].cluster)
			{
				if (firstGlyph)
				{
					unsafeToBreak = false;
					unsafeToConcat = false;
					firstGlyph = false;
				}

				unsafeToBreak |= glyphs[glyphIndex].unsafeToBreak;
				unsafeToConcat |= glyphs[glyphIndex].unsafeToConcat;
				++glyphIndex;
			}

			if (unsafeToBreak)				
				Console::Write(" ");				
			else if (unsafeToConcat)
				Console::Write("\\");
			else
				Console::Write("+");
		}
		Console::Write("\n");
		


	}

}
