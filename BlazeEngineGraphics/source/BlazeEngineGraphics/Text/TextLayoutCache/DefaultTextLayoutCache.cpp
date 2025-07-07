#include "pch.h"
#include "BlazeEngineCore/Debug/Logger.h"
#include "BlazeEngineGraphics/Text/TextLayoutCache/DefaultTextLayoutCache.h"
#include "BlazeEngine/Resources/Font/FontFace.h"

#include <harfbuzz/hb-ft.h>
#include <harfbuzz/hb.h>
#include <linebreak.h>

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
		StyleRunIterator(ArrayView<TextStyleSpan> styleSpans, uintMem lastRunEndIndex, ResourceRef<TextStyle> defaultTextStyle)
			: styleSpans(styleSpans), lastRunEndIndex(lastRunEndIndex), defaultTextStyle(defaultTextStyle), runData(), spanIndex(0)
		{
			runData.startIndex = 0;

			if (styleSpans.Empty())
			{
				runData.endIndex = lastRunEndIndex;
				runData.fontFace = defaultTextStyle->fontFace;
				runData.fontSize = defaultTextStyle->fontSize;
			}
			else if (styleSpans[0].startIndex == 0)
			{
				runData.endIndex = styleSpans[0].endIndex;
				runData.fontFace = styleSpans[0].style->fontFace;
				runData.fontSize = styleSpans[0].style->fontSize;
			}
			else
			{
				runData.endIndex = styleSpans[0].startIndex;
				runData.fontFace = defaultTextStyle->fontFace;
				runData.fontSize = defaultTextStyle->fontSize;
			}
		}

		bool IsValid() const
		{
			return (styleSpans.Empty() && spanIndex == 0 || !styleSpans.Empty()) && runData.endIndex == lastRunEndIndex;
		}

		StyleRunIterator& operator++()
		{
			if (styleSpans.Empty())
			{
				if (spanIndex == 1)
					BLAZE_ENGINE_GRAPHICS_ERROR("Increasing a style run iterator that has reached the end");

				spanIndex = 1;
			}
			else if (!FindNextCombinedStyleRun(runData, spanIndex, styleSpans, lastRunEndIndex, defaultTextStyle))
				BLAZE_ENGINE_GRAPHICS_ERROR("Increasing a style run iterator that has reached the end");

			return *this;
		}
		const StyleRunData* operator->() const { return &runData; }
		const StyleRunData& operator*() const { return runData; }
	private:
		StyleRunData runData;
		uintMem spanIndex;

		const ArrayView<TextStyleSpan> styleSpans;
		const uintMem lastRunEndIndex;
		ResourceRef<TextStyle> defaultTextStyle;

		/*
		Finds the next style span run. If there is no new run false is returned, true otherwise. The new run
		can be empty (runStartIndex == runEndIndex), in that case call the function until the run isn't
		empty if a non-empty run is needed. styleSpans mustn't be empty. Initial value of runStartIndex must
		be 0, runEndIndex must be the beginning of the first span, and styleSpanIndex must be 0.
		isBetweenStyleSpans is only used as output.

		\param runStartIndex - the index of the beginning of the run. Written value is the beginning of the
		next run. Initial value must be 0
		\param runEndIndex - the index of the end of the run (exclusive). Written value is the end of the
		next run. Initial value must be the beginning of the first span
		\param styleSpanIndex - the index of the current span of the run or the next span if the returned
		value of isBetweenStyleSpans is true. Initial value must be 0
		\param isBetweenStyleSpans - a bool denoting wether the new run is between style spans
		\param styleSpans - the style spans. Mustn't be empty
		\param lastRunEndIndex - the index which denotes the end of runs. Practically the size of the text

		\returns True if there is a new style, false otherwise
	*/
		static bool FindNextStyleRun(uintMem& runStartIndex, uintMem& runEndIndex, uintMem& styleSpanIndex, bool& isBetweenStyleSpans, ArrayView<TextStyleSpan> styleSpans, uintMem lastRunEndIndex)
		{
			//TODO what if styleSpans is empty

			if (runEndIndex == styleSpans[styleSpanIndex].endIndex && runEndIndex != runStartIndex)
			{
				//The previous run was within a style span so we start a run between style spans
				++styleSpanIndex;

				runStartIndex = runEndIndex;

				if (styleSpanIndex == styleSpans.Count())
					runEndIndex = lastRunEndIndex; //This is the run after all spans so it goes to the end
				else
					runEndIndex = std::min(styleSpans[styleSpanIndex].startIndex, lastRunEndIndex); //Start a run that ends at the beginning of the next span 

				isBetweenStyleSpans = true;
			}
			else
			{
				//The previous run was between style spans so we start a run within a style span

				if (runEndIndex == lastRunEndIndex) //There are no more runs so we exit with false
					return false;

				//Start a run within a style span
				runStartIndex = runEndIndex;
				runEndIndex = std::min(styleSpans[styleSpanIndex].endIndex, lastRunEndIndex);

				isBetweenStyleSpans = false;
			}

			return true;
		}
		/*
		Finds the next combined style span run, meaning if multiple style spans contain styles that have
		matching font faces and font sizes they are combined into a one contineous run. Runs between the
		style spans are treated like they have the defaultTextStyle. Returned runs are never empty
		(runStartIndex != runEndIndex), except when there is no next run, then false is returned. If there
		is a new run true is returned. styleSpans mustn't be empty. Initial conditions must be the same
		as for FindNextStyleRun.

		\param runStartIndex - the index of the beginning of the run. Written value is the beginning of the
		next run. Initial value must be 0
		\param runEndIndex - the index of the end of the run (exclusive). Written value is the end of the
		next run. Initial value must be the beginning of the first span
		\param styleSpanIndex - the index of the current span of the run or the next span if the returned
		run is between style spans. Initial value must be 0
		\param styleSpans - the style spans. Mustn't be empty
		\param lastRunEndIndex - the index which denotes the end of runs. Practically the size of the text
		\paran defaultTextStyle - the default text style. All runs between style spans are considered to
		use this text style

		\returns True if there is a new style, false otherwise
		*/
		static bool FindNextCombinedStyleRun(StyleRunData& runData, uintMem& spanIndex, ArrayView<TextStyleSpan> styleSpans, uintMem lastRunEndIndex, ResourceRef<TextStyle> defaultTextStyle)
		{
			bool foundNextRun = false;
			bool isBetweenStyleSpans;

			while (true)
			{
				uintMem nextRunStartIndex = runData.startIndex;
				uintMem nextRunEndIndex = runData.endIndex;
				uintMem nextStyleSpanIndex = spanIndex;

				//Find the next style span run that isn't empty
				bool outOfRuns = false;
				do
					if (!FindNextStyleRun(nextRunStartIndex, nextRunEndIndex, nextStyleSpanIndex, isBetweenStyleSpans, styleSpans, lastRunEndIndex))
					{
						outOfRuns = true;
						break;
					}
				while (nextRunStartIndex == nextRunEndIndex);

				//A non-empty run wasn't found so we exit the loop
				if (outOfRuns)
					break;

				foundNextRun = true;

				auto nextFontFace = isBetweenStyleSpans ? defaultTextStyle->fontFace : styleSpans[nextStyleSpanIndex].style->fontFace;
				auto nextFontSize = isBetweenStyleSpans ? defaultTextStyle->fontSize : styleSpans[nextStyleSpanIndex].style->fontSize;

				//Compare the current and next styles. If they match extend the run and continue trying to combine with the next runs
				if (runData.fontFace == nextFontFace && runData.fontSize == nextFontSize)
				{
					runData.endIndex = nextRunEndIndex;
					spanIndex = nextStyleSpanIndex;
				}
				else
					break;
			}

			if (!foundNextRun)
				return false;

			if (isBetweenStyleSpans)
			{
				runData.fontFace = defaultTextStyle->fontFace;
				runData.fontSize = defaultTextStyle->fontSize;
			}
			else
			{
				runData.fontFace = styleSpans[spanIndex].style->fontFace;
				runData.fontSize = styleSpans[spanIndex].style->fontSize;
			}

			return true;
		}
	};

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

	auto Blaze::DefaultTextLayoutCache::ShapeText() -> TextShapingData
	{
		Array<TextShapingData::ClusterShapingData> clusters;
		Array<TextShapingData::GlyphShapingData> glyphs;

		clusters.ReserveExactly(GetString().CharacterCount());

		StyleRunIterator styleRunIterator{ GetStyleSpans(), GetString().CharacterCount(), GetDefaultStyle() };
		for (; styleRunIterator.IsValid(); ++styleRunIterator)
		{
			hb_buffer_t* buffer = nullptr;

			buffer = hb_buffer_create();
			hb_buffer_add_utf8(buffer, (const char*)GetString().Buffer(), GetString().CharacterCount(), styleRunIterator->startIndex, styleRunIterator->endIndex - styleRunIterator->startIndex);
			hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
			hb_buffer_guess_segment_properties(buffer);
			
			FT_Face ft_face = (FT_Face)styleRunIterator->fontFace->GetHandle();
			FT_Set_Pixel_Sizes(ft_face, (FT_UInt)styleRunIterator->fontSize, 0);
						
			hb_font_t* font = hb_ft_font_create(ft_face, NULL);

			hb_ft_font_set_load_flags(font, FT_LOAD_NO_BITMAP);
			
			hb_shape(font, buffer, NULL, 0);

			uint glyphCount;
			hb_glyph_info_t* glyphInfo = hb_buffer_get_glyph_infos(buffer, &glyphCount);
			hb_glyph_position_t* glyphPos = hb_buffer_get_glyph_positions(buffer, &glyphCount);
			hb_codepoint_t invisibleGlyph = hb_buffer_get_invisible_glyph(buffer);			

			glyphs.ReserveAdditional(glyphCount);
			
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

			hb_buffer_destroy(buffer);
			hb_font_destroy(font);	
		}

		for (auto& styleSpan : GetStyleSpans())
			for (uintMem i = styleSpan.startIndex; i < styleSpan.endIndex; ++i)
				clusters[i].style = styleSpan.style;

		return TextShapingData{ std::move(clusters), std::move(glyphs) };
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
		FontFaceDirection textDirection = GetDefaultStyle()->fontFace->GetDirection();

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
		FontFaceDirection textDirection = GetDefaultStyle()->fontFace->GetDirection();

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