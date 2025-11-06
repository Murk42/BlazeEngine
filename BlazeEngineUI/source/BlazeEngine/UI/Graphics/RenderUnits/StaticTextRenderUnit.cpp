#include "pch.h"
#include "BlazeEngine/UI/Graphics/RenderUnits/StaticTextRenderUnit.h"
#include "harfbuzz/hb-ft.h"
#include "BlazeEngine/UI/Text/TextShaping/WrappedTextShaping.h"

namespace Blaze::UI
{
	void StaticTextRenderUnit::Clear()
	{
		atlas = {};
		glyphs.Clear();
	}
	Vec2f StaticTextRenderUnit::CopyRenderData(ArrayView<TextShaping::ShapedText> shapedText, float fontSize, const FontAtlas& fontAtlas)
	{
		if (fontAtlas.GetFontFace() == nullptr)
		{
			BLAZE_LOG_ERROR("Cannot copy render data with an empty font atlas.");
			return Vec2f();
		}

		auto& fontFace = *fontAtlas.GetFontFace();

		FT_Face fontFaceHandle = (FT_Face)fontFace.GetHandle();

		uintMem glyphCount = 0;
		for (auto& line : shapedText)
			glyphCount += line.glyphs.Count();

		glyphs.Resize(glyphCount);
		atlas = fontAtlas.GetTexture();

		float maxWidth = 0.0f;
		Vec2f advance{ 0, -fontSize };
		auto it = glyphs.Ptr();
		for (const auto& line : shapedText)
		{
			advance.x = 0;

			for (const auto& glyph : line.glyphs)
			{
				FT_Load_Glyph(fontFaceHandle, glyph.glyphIndex, FT_LOAD_COMPUTE_METRICS | FT_LOAD_NO_BITMAP);

				it->offset = advance + Vec2f(glyph.offset) + Vec2f(
					static_cast<float>(fontFaceHandle->glyph->metrics.horiBearingX),
					static_cast<float>(fontFaceHandle->glyph->metrics.horiBearingY - fontFaceHandle->glyph->metrics.height)) / 64;

				Vec2u16 uv1, uv2;
				if (fontAtlas.GetGlyphUV(glyph.glyphIndex, uv1, uv2))
				{
					it->uv1 = Vec2f(uv1) / fontAtlas.GetSize();
					it->uv2 = Vec2f(uv2) / fontAtlas.GetSize();
					it->size = { static_cast<float>(fontFaceHandle->glyph->metrics.width / 64) , static_cast<float>(fontFaceHandle->glyph->metrics.height / 64) };
				}
				else
				{
					it->uv1 = Vec2f();
					it->uv2 = Vec2f();
					it->size = Vec2f();
				}

				it->offset = Vec2f(Math::Floor(it->offset.x), Math::Floor(it->offset.y));
				it->size = Vec2f(Math::Floor(it->size.x), Math::Floor(it->size.y));

				advance += Vec2f(glyph.advance) / 64;

				++it;
			}

			maxWidth = line.extent / 64 > maxWidth ? line.extent / 64 : maxWidth;
			advance.y -= fontSize;
		}

		return { maxWidth, shapedText.Count() * fontSize };
	}
	Vec2f StaticTextRenderUnit::BuildText(u8StringView string, float fontSize, const FontAtlas& atlas)
	{
		if (atlas.GetFontFace() == nullptr)
		{
			BLAZE_LOG_ERROR("Cannot build text render data with an empty font atlas.");
			return Vec2f();
		}

		auto& fontFace = *atlas.GetFontFace();

		FT_Face fontFaceHandle = (FT_Face)fontFace.GetHandle();
		FT_Set_Pixel_Sizes(fontFaceHandle, static_cast<FT_UInt>(fontSize), 0);

		u32String convertedString = string.ConvertToStringType<char32_t>();
		TextShaping::ShapingContext shapingContext{ convertedString, fontFace };

		auto shapedText = TextShaping::ShapeText(convertedString.FirstIterator(), convertedString.BehindIterator(), shapingContext);
		return CopyRenderData({ &shapedText, 1 }, fontSize, atlas);
	}
	Vec2f StaticTextRenderUnit::BuildWrappedText(u8StringView string, float fontSize, const FontAtlas& atlas, float wrapWidth)
	{
		if (atlas.GetFontFace() == nullptr)
		{
			BLAZE_LOG_ERROR("Cannot build text render data with an empty font atlas.");
			return Vec2f();
		}

		auto& fontFace = *atlas.GetFontFace();

		FT_Face fontFaceHandle = (FT_Face)fontFace.GetHandle();
		FT_Set_Pixel_Sizes(fontFaceHandle, static_cast<FT_UInt>(fontSize), 0);

		u32String convertedString = string.ConvertToStringType<char32_t>();
		TextShaping::WrappedTextShapingContext shapingContext{ convertedString, fontFace, static_cast<uint32>(wrapWidth * 64) };

		auto shapedText = TextShaping::ShapeTextWrapped(convertedString.FirstIterator(), convertedString.BehindIterator(), shapingContext);
		return CopyRenderData(shapedText, fontSize, atlas);
	}
	void StaticTextRenderUnit::Render(const Node& node, Graphics::TexturedRectRenderer& renderer, const UIRenderContext& renderContext)
	{
		Vec2f up = { -right.y, right.x };
		Vec2f roundedPos = Math::Floor(position);

		for (auto& glyph : glyphs)
		{
			renderer.Render({
				.texture = atlas,
				.uv1 = glyph.uv1,
				.uv2 = glyph.uv2,
				.pos = roundedPos + glyph.offset,
				.right = right * glyph.size.x,
				.up = up * glyph.size.y,
				.color = 0xffffffff,
				.blend = 0.0f,
				.alpha = 1.0f,
				}, renderContext);
		}
	}
}