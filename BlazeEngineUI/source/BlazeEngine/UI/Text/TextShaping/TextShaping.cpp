#include "pch.h"
#include "BlazeEngine/UI/Text/TextShaping/TextShaping.h"
#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>

namespace Blaze::UI::TextShaping
{
	ShapingContext::ShapingContext(u8StringView text, const FontFace& fontFace, uint32 fontHeight)
		: text(text), fontFace(fontFace), fontHandle(nullptr), fontHeight(fontHeight)
	{
		if (fontFace.IsValidFontFace())
		{
			FT_Face fontFaceHandle = (FT_Face)fontFace.GetHandle();
			FT_Set_Pixel_Sizes(fontFaceHandle, static_cast<FT_UInt>(fontHeight), 0);
			fontHandle = hb_ft_font_create_referenced((FT_Face)fontFace.GetHandle());
		}
		else
		{
			fontHeight = 0;
			BLAZE_LOG_ERROR("Creating a shaping context with an invalid font face");
		}

	}
	TextShaping::ShapingContext::~ShapingContext()
	{
		hb_font_destroy((hb_font_t*)fontHandle);
	}

	static uint32 ShapeTextGlyphs(u8StringView text, const ShapingContext& shapingContext, Array<GlyphInfo>& glyphs)
	{
		hb_buffer_t* buf = hb_buffer_create();
		//hb_buffer_add_utf32(buf, reinterpret_cast<const uint32*>(shapingContext.GetText().Ptr()), static_cast<int>(shapingContext.GetText().Count()), static_cast<unsigned>(text.FirstIterator() - shapingContext.GetText().FirstIterator()), static_cast<int>(text.Count()));
		hb_buffer_add_utf8(buf, reinterpret_cast<const char*>(shapingContext.GetText().Ptr()), static_cast<int>(shapingContext.GetText().Count()), static_cast<unsigned>(text.FirstIterator() - shapingContext.GetText().FirstIterator()), static_cast<int>(text.Count()));
		hb_buffer_guess_segment_properties(buf);
		hb_buffer_set_flags(buf, HB_BUFFER_FLAG_PRODUCE_UNSAFE_TO_CONCAT);
		hb_buffer_set_direction(buf, HB_DIRECTION_LTR);

		hb_shape(static_cast<hb_font_t*>(shapingContext.GetFontHandle()), buf, nullptr, 0);

		unsigned count;
		const hb_glyph_info_t* info = hb_buffer_get_glyph_infos(buf, &count);
		const hb_glyph_position_t* pos = hb_buffer_get_glyph_positions(buf, &count);

		glyphs.ReserveAdditional(count);
		uint32 extent = 0;

		uint32 clusterBegin = UINT32_MAX;
		uint32 clusterEnd = clusterBegin;
		for (unsigned i = 0; i < count; ++i)
		{
			uint32 currentCluster = info[i].cluster;
			if (currentCluster != clusterBegin)
			{
				clusterBegin = currentCluster;
				clusterEnd = clusterBegin;

				unsigned j = i;
				while (j < count && info[j].cluster == currentCluster)
					++j;

				if (j == count)
					clusterEnd = static_cast<uint32>(text.BehindIterator() - shapingContext.GetText().FirstIterator());
				else
					clusterEnd = info[j].cluster;
			}

			glyphs.AddBack(GlyphInfo{
				.textBegin = shapingContext.GetText().Ptr() + static_cast<size_t>(clusterBegin),
				.textEnd = shapingContext.GetText().Ptr() + static_cast<size_t>(clusterEnd),
				.glyphIndex = info[i].codepoint,
				.advance = Vec2f{ static_cast<float>(pos[i].x_advance), static_cast<float>(pos[i].y_advance) } / 64.0f,
				.offset = Vec2f{ static_cast<float>(pos[i].x_offset), static_cast<float>(pos[i].y_offset) } / 64.0f,
				.unsafeToBreak = static_cast<bool>(hb_glyph_info_get_glyph_flags(info + i) & HB_GLYPH_FLAG_UNSAFE_TO_BREAK),
				.unsafeToConcat = static_cast<bool>(hb_glyph_info_get_glyph_flags(info + i) & HB_GLYPH_FLAG_UNSAFE_TO_CONCAT),
				});
			extent += pos[i].x_advance;
		}

		hb_buffer_destroy(buf);

		return float(extent) / 64.0f;
	}

	ShapingResult ShapeText(u8StringView text, const ShapingContext& shapingContext)
	{
		if (text.FirstIterator() < shapingContext.GetText().FirstIterator() || text.BehindIterator() > shapingContext.GetText().BehindIterator())
		{
			BLAZE_LOG_ERROR("The text to shape must be a subrange of the text in the shaping context");
			return { };
		}

		if (shapingContext.GetFontHandle() == nullptr)
		{
			BLAZE_LOG_ERROR("Trying to shape a text with an invalid font handle");
			return { };
		}		

		if (text.Empty())
			return { };

		ShapingResult result;
		result.extent = ShapeTextGlyphs(text, shapingContext, result.glyphs);

		return result;
	}
	
}