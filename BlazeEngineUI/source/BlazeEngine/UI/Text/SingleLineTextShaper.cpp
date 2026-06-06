#include "pch.h"
#include "BlazeEngine/UI/Text/SingleLineTextShaper.h"
#include "BlazeEngine/UI/Text/FontFace.h"
#include <harfbuzz/hb-ft.h>

namespace Blaze::UI::Text
{	
	SingleLineTextShaper::SingleLineTextShaper(u8StringView textContext, const Nodes::TextStyle& textStyle)
		: textContext(textContext), fontHandle(nullptr), textStyle(textStyle)
	{
		auto fontData = textStyle.GetFontData();
		if (fontData.fontFace == nullptr)
		{
			BLAZE_LOG_ERROR("Creating a shaping context with an invalid text style");
			return;
		}

		if (fontData.fontFace->IsValidFontFace())
		{
			FT_Face fontFaceHandle = (FT_Face)fontData.fontFace->GetHandle();
			FT_Set_Pixel_Sizes(fontFaceHandle, static_cast<FT_UInt>(textStyle.fontHeight), 0);
			fontHandle = hb_ft_font_create_referenced((FT_Face)fontData.fontFace->GetHandle());
		}
		else
		{
			BLAZE_LOG_ERROR("Creating a shaping context with an invalid font face");
		}
	}
	SingleLineTextShaper::~SingleLineTextShaper()
	{
		hb_font_destroy((hb_font_t*)fontHandle);
	}
	Array<ShapedString> SingleLineTextShaper::Shape(u8StringView text)
	{
		Array<ShapedString> resultArray;
		resultArray.AddBack(ShapeSingleLine(text));

		return resultArray;
	}
	ShapedString SingleLineTextShaper::ShapeSingleLine(u8StringView text)
	{
		if (text.FirstIterator() < textContext.FirstIterator() || text.BehindIterator() > textContext.BehindIterator())
		{
			BLAZE_LOG_ERROR("The text to shape must be a subrange of the text in the shaping context");
			return { };
		}

		if (fontHandle == nullptr)
		{
			BLAZE_LOG_ERROR("Trying to shape a text with an invalid font handle");
			return { };
		}

		if (text.Empty())
			return { };

		ShapedString result;

		hb_buffer_t* buf = hb_buffer_create();
		hb_buffer_add_utf8(buf, reinterpret_cast<const char*>(textContext.Ptr()), static_cast<int>(textContext.Count()), static_cast<unsigned>(text.FirstIterator() - textContext.FirstIterator()), static_cast<int>(text.Count()));
		hb_buffer_guess_segment_properties(buf);
		hb_buffer_set_flags(buf, HB_BUFFER_FLAG_PRODUCE_UNSAFE_TO_CONCAT);
		hb_buffer_set_direction(buf, HB_DIRECTION_LTR);

		hb_shape(static_cast<hb_font_t*>(fontHandle), buf, nullptr, 0);

		unsigned count;
		const hb_glyph_info_t* info = hb_buffer_get_glyph_infos(buf, &count);
		const hb_glyph_position_t* pos = hb_buffer_get_glyph_positions(buf, &count);

		result.glyphs.ReserveAdditional(count);
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
					clusterEnd = static_cast<uint32>(text.BehindIterator() - textContext.FirstIterator());
				else
					clusterEnd = info[j].cluster;
			}

			result.glyphs.AddBack(ShapedGlyph{
				.textBegin = static_cast<size_t>(clusterBegin),
				.textEnd = static_cast<size_t>(clusterEnd),
				.glyphIndex = info[i].codepoint,
				.advance = Vec2f{ static_cast<float>(pos[i].x_advance), static_cast<float>(pos[i].y_advance) } / 64.0f,
				.offset = Vec2f{ static_cast<float>(pos[i].x_offset), static_cast<float>(pos[i].y_offset) } / 64.0f,
				.unsafeToBreak = static_cast<bool>(hb_glyph_info_get_glyph_flags(info + i) & HB_GLYPH_FLAG_UNSAFE_TO_BREAK),
				.unsafeToConcat = static_cast<bool>(hb_glyph_info_get_glyph_flags(info + i) & HB_GLYPH_FLAG_UNSAFE_TO_CONCAT),
				});
			extent += pos[i].x_advance;
		}

		hb_buffer_destroy(buf);

		result.extent = float(extent) / 64.0f;

		return result;
	}
}
