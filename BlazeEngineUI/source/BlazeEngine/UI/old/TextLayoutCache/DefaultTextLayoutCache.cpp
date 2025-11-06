#include "pch.h"
#include "BlazeEngineGraphics/Text/TextLayoutCache/DefaultTextLayoutCache.h"

#include "BlazeEngineCore/Debug/Logger.h"
#include "BlazeEngine/Resources/Font/FontFace.h"
#include "BlazeEngineGraphics/Text/Core/StyleRunIterator.h"

#include <harfbuzz/hb-ft.h>
#include <harfbuzz/hb.h>
#include <linebreak.h>

namespace Blaze
{	
	enum class LineBreakAttribute : uint8
	{
		AllowBreak,
		NoBreak,
		MustBreak
	};
	static Array<LineBreakAttribute> GetLineBreakingAttributes(StringViewUTF8 text, const char* language)
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

	static void GetStyleHorizontalFontMetrics(const TextStyle& style, float& height, float& ascent, float& descent)
	{
		FT_Face ft_face = (FT_Face)style.fontFace->GetHandle();
		FT_Set_Pixel_Sizes(ft_face, (FT_UInt)style.fontSize, 0);

		height = float(ft_face->size->metrics.height) / 64;
		ascent = float(ft_face->size->metrics.ascender) / 64;
		descent = float(ft_face->size->metrics.descender) / 64;
	}
	static void GetStyleVerticalFontMetrics(const TextStyle& style, float& width)
	{
		FT_Face ft_face = (FT_Face)style.fontFace->GetHandle();
		FT_Set_Pixel_Sizes(ft_face, (FT_UInt)style.fontSize, 0);

		width = ft_face->size->metrics.max_advance;
	}

	DefaultTextLayoutCache::DefaultTextLayoutCache()
		: lineWrapLength(FLT_MAX)
	{		
	}

	void DefaultTextLayoutCache::SetLineWrapLength(float lineWrapLength)
	{
		if (this->lineWrapLength != lineWrapLength)
		{
			this->lineWrapLength = lineWrapLength;
			SetDirtyState(DirtyState::PartitionLines);
		}
	}

	using ClusterShapingData = TextLayoutCacheBase::TextShapingData::ClusterShapingData;
	using GlyphShapingData = TextLayoutCacheBase::TextShapingData::GlyphShapingData;

	static void ExtractShapingData(hb_buffer_t* buffer, FT_Face face, Array<TextLayoutCacheBase::GlyphShapingData>& glyphs)
	{
		uint glyphCount;
		hb_glyph_info_t* glyphInfo = hb_buffer_get_glyph_infos(buffer, &glyphCount);
		hb_glyph_position_t* glyphPos = hb_buffer_get_glyph_positions(buffer, &glyphCount);

		struct GlyphMetaData
		{
			bool concat;
			bool breakable;
		};

		Array<GlyphMetaData> glyphMetaData{ glyphCount };
		
		glyphs.ReserveAdditional(glyphCount);
		for (uintMem i = 0; i < glyphCount; ++i)
		{
			glyphs.AddBack(TextLayoutCacheBase::GlyphShapingData{
				.glyphIndex = glyphInfo[i].codepoint,
				.advance = Vec2i16(glyphPos[i].x_advance, glyphPos[i].y_advance),
				.offset = Vec2i16(glyphPos[i].x_offset, glyphPos[i].y_offset)
				});

			glyphMetaData[i].breakable = hb_glyph_info_get_glyph_flags(glyphInfo + i) & HB_GLYPH_FLAG_UNSAFE_TO_BREAK;
			glyphMetaData[i].concat = hb_glyph_info_get_glyph_flags(glyphInfo + i) & HB_GLYPH_FLAG_UNSAFE_TO_CONCAT;
		}

	}

	static void ShapeSubString(StringViewUTF8 subString, const FontFace& fontFace, float fontSize, Array<TextLayoutCacheBase::GlyphShapingData>& glyphs, bool isBeginning, bool isEnd)
	{
		hb_buffer_t* buffer = hb_buffer_create();
		
		//Add all of the unicode characters from the text to the HarfBuzz buffer, each referenced by its
		//character index by the cluster value
		hb_buffer_pre_allocate(buffer, subString.CharacterCount());
		uintMem i = 0;
		for (const auto& character : subString)
		{
			hb_buffer_add(buffer, character.Value(), i);
			++i;
		}

		//TODO multi directional text isn't supported
		hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
		hb_buffer_guess_segment_properties(buffer);
		hb_buffer_set_flags(buffer, hb_buffer_flags_t(
			HB_BUFFER_FLAG_PRODUCE_UNSAFE_TO_CONCAT |
			(isBeginning ? HB_BUFFER_FLAG_BOT : 0) |
			(isEnd ? HB_BUFFER_FLAG_EOT : 0)
		));

		//Get the FreeType face and sets its size (it's requires to set the font face size by the 
		//hb_ft_font_create function
		FT_Face ft_face = (FT_Face)fontFace.GetHandle();
		FT_Set_Pixel_Sizes(ft_face, (FT_UInt)fontSize, 0);

		hb_font_t* font = hb_ft_font_create(ft_face, NULL);
		hb_ft_font_set_load_flags(font, FT_LOAD_NO_BITMAP);

		hb_shape(font, buffer, NULL, 0);

		/*
		uintMem cluster = SIZE_MAX;
		for (uint i = 0; i < glyphCount; i++)
		{
			if (cluster != glyphInfo[i].cluster)
			{
				uintMem textStartIndex = styleRunIterator->startIndex + glyphInfo[i].cluster;

				if (!clusters.Empty())
				{
					auto& prev = clusters.Last();
					prev.textEndIndex = textStartIndex;
					prev.glyphEndIndex = glyphs.Count();
				}

				clusters.AddBack(TextShapingData::ClusterShapingData{
					.textStartIndex = textStartIndex,
					.textEndIndex = textStartIndex, //This will be filled properly when the cluster ends
					.glyphStartIndex = glyphs.Count(),
					.glyphEndIndex = glyphs.Count(), //This will be filled properly when the cluster ends
					.style = GetDefaultStyle(),
					.advance = Vec2f(),
					});
			}

			FT_Load_Glyph(ft_face, glyphInfo[i].codepoint, FT_LOAD_NO_BITMAP);

			Vec2i16 shapingOffset = Vec2i16((glyphPos[i].x_offset + 32) / 64, (glyphPos[i].y_offset + 32) / 64);
			Vec2i16 glyphOffset = Vec2i16((int16)ft_face->glyph->metrics.horiBearingX / 64, (int16)ft_face->glyph->metrics.horiBearingY / 64 - (int16)ft_face->glyph->metrics.height / 64);

			glyphs.AddBack(TextShapingData::GlyphShapingData{
				.glyphIndex = glyphInfo[i].codepoint,
				.size = Vec2u16((uint16)ft_face->glyph->metrics.width / 64, (uint16)ft_face->glyph->metrics.height / 64),
				.clusterOffset = Vec2i16(clusters.Last().advance) + glyphOffset + shapingOffset
				});

			clusters.Last().advance += Vec2f((glyphPos[i].x_advance + 32) / 64, (glyphPos[i].y_advance + 32) / 64);

		}

		clusters.Last().glyphEndIndex = glyphs.Count();
		clusters.Last().textEndIndex = styleRunIterator->endIndex;
		*/


		ExtractShapingData(buffer, ft_face, glyphs);

		hb_buffer_destroy(buffer);
		hb_font_destroy(font);
	}
	static void ShapeText(TextContainerBase* textContainer, TextStyleData* textStyleData, ResourceRef<TextStyle> defaultTextStyle)
	{
		Array<TextLayoutCacheBase::GlyphShapingData> glyphs;

		uintMem textIteratorIndex = 0;		

		StyleRunIterator styleRunIterator{ textStyleData->GetAllStyleSpans(), textContainer->CharacterCount(), defaultTextStyle };
		for (; styleRunIterator.IsValid(); ++styleRunIterator)
		{
			StringViewUTF8 subString = textContainer->GetSubString(styleRunIterator->startIndex, styleRunIterator->endIndex - styleRunIterator->startIndex);

			ShapeSubString(subString, *styleRunIterator->fontFace, styleRunIterator->fontSize, glyphs);
		}
	}
	auto DefaultTextLayoutCache::ShapeText() -> TextShapingData
	{
		Array<TextLayoutCacheBase::GlyphShapingData> glyphs;

		uintMem textIteratorIndex = 0;

		auto textContainer = GetTextContainer();
		auto textStyleData = GetTextStyleData();		

		StyleRunIterator styleRunIterator{ textStyleData->GetAllStyleSpans(), textContainer->CharacterCount(), GetDefaultTextStyle()};
		for (; styleRunIterator.IsValid(); ++styleRunIterator)
		{
			StringViewUTF8 subString = textContainer->GetSubString(styleRunIterator->startIndex, styleRunIterator->endIndex - styleRunIterator->startIndex);

			ShapeSubString(subString, *styleRunIterator->fontFace, styleRunIterator->fontSize, glyphs);
		}
	}
	auto DefaultTextLayoutCache::CalculateClusterBounds() -> Array<ClusterBoundsData>
	{
		auto& shapingData = GetTextShapingData();

		Array<ClusterBoundsData> boundsData;
		boundsData.Resize(shapingData.clusters.Count());

		for (uintMem i = 0; i < shapingData.clusters.Count(); ++i)
		{
			auto& bounds = boundsData[i];
			auto& cluster = shapingData.clusters[i];

			bounds.maxX = INT16_MIN;
			bounds.maxY = INT16_MIN;
			bounds.minX = INT16_MAX;
			bounds.minY = INT16_MAX;

			for (uintMem j = cluster.glyphStartIndex; j < cluster.glyphEndIndex; ++j)
			{
				Vec2i16 p1 = shapingData.glyphs[j].clusterOffset;
				Vec2i16 p2 = p1 + shapingData.glyphs[j].size;

				bounds.maxX = p2.x;
				bounds.maxY = p2.y;
				bounds.minX = p1.x;
				bounds.minY = p1.y;
			}
		}

		return boundsData;		
	}
	auto DefaultTextLayoutCache::PartitionLines() -> Array<LinePartitionData>
	{
		auto& shapingData = GetTextShapingData();
		auto clusterBoundsData = GetClusterBoundsData();
		FontFaceDirection textDirection = GetDefaultStyle()->GetFontFace()->GetDirection();

		Array<LineBreakAttribute> lineBreakAttributes = GetLineBreakingAttributes(GetString(), hb_language_to_string(hb_language_get_default()));
		Array<LinePartitionData> linePartitionData;

		float cursor = 0;
		uintMem currentTextIndex = 0;
		uintMem currentClusterIndex = 0;

		uintMem newLineTextStartIndex = 0;
		uintMem newLineTextEndIndex = 0;
		uintMem newLineClusterStartIndex = 0;
		uintMem newLineClusterEndIndex = 0;

		auto NewLine = [&]() {
			auto& newLine = *linePartitionData.AddBack();
			newLine.textStart = newLineTextStartIndex;
			newLine.textEnd = newLineTextEndIndex;
			newLine.clusterStart = newLineClusterStartIndex;
			newLine.clusterEnd = newLineClusterEndIndex;

			newLineTextStartIndex = newLineTextEndIndex;
			newLineClusterStartIndex = newLineClusterEndIndex;

			currentTextIndex = newLineTextEndIndex;
			currentClusterIndex = newLineClusterEndIndex;
			};

		while (currentTextIndex < lineBreakAttributes.Count())
		{
			const TextShapingData::ClusterShapingData* currentClusterLayoutData =
				currentClusterIndex != shapingData.clusters.Count() && 
				currentTextIndex + 1 == shapingData.clusters[currentClusterIndex].textEndIndex ? &shapingData.clusters[currentClusterIndex] : nullptr;

			bool breakLine = false;

			switch (lineBreakAttributes[currentTextIndex])
			{
			case LineBreakAttribute::AllowBreak:
				newLineTextEndIndex = currentTextIndex + 1;
				newLineClusterEndIndex = currentClusterIndex + 1;

				if (currentClusterLayoutData != nullptr && (
					textDirection == FontFaceDirection::Horizontal && (cursor + clusterBoundsData[currentClusterIndex].maxX > lineWrapLength) ||
					textDirection == FontFaceDirection::Vertical && (-cursor + clusterBoundsData[currentClusterIndex].minY > lineWrapLength)
					))
				{
					breakLine = true;
				}

				break;
			case LineBreakAttribute::MustBreak:
				newLineTextEndIndex = currentTextIndex + 1;
				newLineClusterEndIndex = currentClusterIndex + 1;
				breakLine = true;
				break;
			case LineBreakAttribute::NoBreak:
				break;
			}

			if (breakLine)
			{
				NewLine();

				cursor = 0;
			}
			else
			{
				if (currentClusterLayoutData != nullptr)
				{
					cursor += textDirection == FontFaceDirection::Horizontal ? currentClusterLayoutData->advance.x : currentClusterLayoutData->advance.y;
					++currentClusterIndex;
				}
				++currentTextIndex;
			}
		}

		NewLine();

		return linePartitionData;
	}
	auto DefaultTextLayoutCache::LayoutText() -> TextLayoutData
	{
		auto& shapingData = GetTextShapingData();
		auto clusterBoundsData = GetClusterBoundsData();
		auto linesPartitionData = GetLinePartitionData();
		FontFaceDirection textDirection = GetDefaultStyle()->GetFontFace()->GetDirection();

		TextLayoutData layoutData;
		layoutData.clusters.Resize(shapingData.clusters.Count());
		layoutData.lines.Resize(linesPartitionData.Count());

		Vec2f lineCursor = Vec2f();
		for (uintMem i = 0; i < linesPartitionData.Count(); ++i)
		{
			auto& linePartitionData = linesPartitionData[i];

			float ascent = 0.0f;
			float descent = 0.0f;
			Vec2f lineSize = Vec2f();

			Vec2f lineAdvance = Vec2f();
			if (linePartitionData.clusterStart != linePartitionData.clusterEnd)
			{
				for (uintMem j = linePartitionData.clusterStart; j < linePartitionData.clusterEnd - 1; ++j)
				{
					layoutData.clusters[j].lineOffset = lineAdvance;
					layoutData.clusters[j].lineIndex = i;
					lineAdvance += shapingData.clusters[j].advance;


					if (textDirection == FontFaceDirection::Horizontal)
					{
						float clusterFontHeight, clusterFontAscent, clusterFontDescent;

						GetStyleHorizontalFontMetrics(shapingData.clusters[j].style.Get(), clusterFontHeight, clusterFontAscent, clusterFontDescent);

						lineSize.y = std::max(lineSize.y, clusterFontHeight);
						ascent = std::max(ascent, clusterFontAscent);
						descent = std::max(descent, clusterFontDescent);
					}
					else
					{
						lineSize.x = std::max(lineSize.x, float(clusterBoundsData[j].maxX - clusterBoundsData[j].minX));
					}
				}

				layoutData.clusters[linePartitionData.clusterEnd - 1].lineOffset = lineAdvance;
				layoutData.clusters[linePartitionData.clusterEnd - 1].lineIndex = i;

				if (textDirection == FontFaceDirection::Horizontal)
				{
					lineAdvance += Vec2f(clusterBoundsData[linePartitionData.clusterEnd - 1].maxX, 0);
					lineSize.x = lineAdvance.x;
				}
				else
				{
					lineAdvance -= Vec2f(0, clusterBoundsData[0].maxX - clusterBoundsData[0].minY);
					lineSize.y = -lineAdvance.y;
				}
			}
			else
			{
				if (textDirection == FontFaceDirection::Horizontal)
				{
					float clusterFontHeight, clusterFontAscent, clusterFontDescent;

					GetStyleHorizontalFontMetrics(GetDefaultStyle().Get(), clusterFontHeight, clusterFontAscent, clusterFontDescent);

					lineSize.y = clusterFontHeight;
					ascent = clusterFontAscent;
					descent = clusterFontDescent;
				}
				else
				{
					float width;
					GetStyleVerticalFontMetrics(GetDefaultStyle().Get(), width);

					lineSize.x = width;
					ascent = 0;
					descent = 0;
				}
			}

			if (textDirection == FontFaceDirection::Horizontal)
			{				
				lineCursor.y -= lineSize.y;
			}
			else
			{
				lineCursor.x += lineSize.x;
			}

			auto& line = layoutData.lines[i];
			line.size = lineSize;
			line.ascent = ascent;
			line.descent = descent;
			line.position = lineCursor;
		}

		return layoutData;
	}

	//TODO set the word breaks
		//TODO Add 'empty' clusters for invisible selectable characters line '\n'
}