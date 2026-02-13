#include "pch.h"
#include "BlazeEngine/UI/Text/TextShaping/TextShaping.h"
#include <harfbuzz/hb.h>
#include "harfbuzz/hb-ft.h"

namespace Blaze::UI::TextShaping
{
	ShapedText ShapeText(u32StringViewIterator begin, u32StringViewIterator end, const ShapingContext& shapingContext)
	{
		if (begin < shapingContext.GetString().FirstIterator() || end > shapingContext.GetString().BehindIterator())
		{
			BLAZE_LOG_ERROR("The text to shape must be a subrange of the text in the shaping context");
			return { };
		}

		if (shapingContext.GetFontHandle() == nullptr)
		{
			BLAZE_LOG_ERROR("Trying to shape a text with an invalid font handle");
			return { };
		}		

		ShapedText result;
		result.textBegin = begin;
		result.textEnd = end;
		result.extent = 0;

		if (begin == end)
			return result;

		hb_buffer_t* buf = hb_buffer_create();
		hb_buffer_add_utf32(buf, reinterpret_cast<const uint32_t*>(shapingContext.GetString().Ptr()), static_cast<int>(shapingContext.GetString().Count()), static_cast<unsigned>(begin - shapingContext.GetString().FirstIterator()), static_cast<int>(end - begin));
		hb_buffer_guess_segment_properties(buf);
		hb_buffer_set_flags(buf, HB_BUFFER_FLAG_PRODUCE_UNSAFE_TO_CONCAT);
		hb_buffer_set_direction(buf, HB_DIRECTION_LTR);

		hb_shape(static_cast<hb_font_t*>(shapingContext.GetFontHandle()), buf, nullptr, 0);

		unsigned count;
		const hb_glyph_info_t* info = hb_buffer_get_glyph_infos(buf, &count);
		const hb_glyph_position_t* pos = hb_buffer_get_glyph_positions(buf, &count);

		result.glyphs.ReserveExactly(count);

		uint32_t clusterBegin = UINT32_MAX;
		uint32_t clusterEnd = clusterBegin;
		for (unsigned i = 0; i < count; ++i)
		{
			uint32_t currentCluster = info[i].cluster;
			if (currentCluster != clusterBegin)
			{
				clusterBegin = currentCluster;
				clusterEnd = clusterBegin;

				unsigned j = i;
				while (j < count && info[j].cluster == currentCluster)
					++j;

				if (j == count)
					clusterEnd = static_cast<uint32>(end - shapingContext.GetString().FirstIterator());
				else
					clusterEnd = info[j].cluster;
			}

			result.glyphs.AddBack(GlyphInfo{
				.textBegin = shapingContext.GetString().Ptr() + static_cast<size_t>(clusterBegin),
				.textEnd = shapingContext.GetString().Ptr() + static_cast<size_t>(clusterEnd),
				.glyphIndex = info[i].codepoint,
				.advance = { pos[i].x_advance, pos[i].y_advance },
				.offset = { pos[i].x_offset, pos[i].y_offset },
				.unsafeToBreak = static_cast<bool>(hb_glyph_info_get_glyph_flags(info + i) & HB_GLYPH_FLAG_UNSAFE_TO_BREAK),
				.unsafeToConcat = static_cast<bool>(hb_glyph_info_get_glyph_flags(info + i) & HB_GLYPH_FLAG_UNSAFE_TO_CONCAT),
				});
			result.extent += pos[i].x_advance;
		}

		hb_buffer_destroy(buf);
		return result;
	}
	ShapingContext::ShapingContext(u32StringView text, const FontFace& fontFace)
		: text(text), fontFace(fontFace), fontHandle(nullptr)
	{
		if (fontFace.IsValidFontFace())
			fontHandle = hb_ft_font_create_referenced((FT_Face)fontFace.GetHandle());
		else
			BLAZE_LOG_ERROR("Creating a shaping context with an invalid font face");
	}
	TextShaping::ShapingContext::~ShapingContext()
	{
		hb_font_destroy((hb_font_t*)fontHandle);
	}
}