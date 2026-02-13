#include "pch.h"
#include "BlazeEngine/UI/Graphics/RenderUnits/StaticTextRenderUnit.h"
#include "harfbuzz/hb-ft.h"
#include "BlazeEngine/UI/Text/TextShaping/WrappedTextShaping.h"
#include "BlazeEngine/UI/Text/FontAtlas.h"

namespace Blaze::UI
{
	StaticTextRenderUnit::StaticTextRenderUnit()
		: rendererTypeID(UINT32_MAX), position(0, 0), right(0, 1), color(0xf5f5f5ff), atlasFontHeight(0.0f), targetFontHeight(0.0f)
	{
	}
	void StaticTextRenderUnit::Clear()
	{
		atlas = {};
		glyphs.Clear();
		rendererTypeID = UINT32_MAX;
		position = { 0, 0 };
		right = { 0, 1 };
		color = 0xf5f5f5ff;
		atlasFontHeight = 0.0f;
		targetFontHeight = 0.0f;
	}
	Vec2f StaticTextRenderUnit::CopyRenderData(ArrayView<TextShaping::ShapedText> shapedText, const FontFace& fontFace, const FontAtlas& fontAtlas)
	{
		FT_Face fontFaceHandle = (FT_Face)fontFace.GetHandle();

		uintMem glyphCount = 0;
		for (auto& line : shapedText)
			glyphCount += line.glyphs.Count();

		glyphs.Resize(glyphCount);
		atlas = fontAtlas.GetTexture();
		atlasFontHeight = fontAtlas.GetRasterFontHeight();

		float maxWidth = 0.0f;
		Vec2f advance{ 0, -fontAtlas.GetRasterFontHeight()};
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
			advance.y -= fontAtlas.GetRasterFontHeight();
		}

		return { maxWidth, shapedText.Count() * fontAtlas.GetRasterFontHeight() };
	}
	Vec2f StaticTextRenderUnit::BuildText(u8StringView string, const FontManager::FontAtlasData& fontAtlasData)
	{
		FT_Face fontFaceHandle = (FT_Face)fontAtlasData.fontFace->GetHandle();
		FT_Set_Pixel_Sizes(fontFaceHandle, static_cast<FT_UInt>(fontAtlasData.atlas->GetRasterFontHeight()), 0);

		u32String convertedString = string.ConvertToStringType<char32_t>();
		TextShaping::ShapingContext shapingContext{ convertedString, *fontAtlasData.fontFace };

		auto shapedText = TextShaping::ShapeText(convertedString.FirstIterator(), convertedString.BehindIterator(), shapingContext);

		rendererTypeID = fontAtlasData.rendererTypeID;

		return CopyRenderData({ &shapedText, 1 }, *fontAtlasData.fontFace, *fontAtlasData.atlas);
	}
	Vec2f StaticTextRenderUnit::BuildWrappedText(u8StringView string, const FontManager::FontAtlasData& fontAtlasData, float wrapWidth)
	{
		FT_Face fontFaceHandle = (FT_Face)fontAtlasData.fontFace->GetHandle();
		FT_Set_Pixel_Sizes(fontFaceHandle, static_cast<FT_UInt>(fontAtlasData.atlas->GetRasterFontHeight()), 0);

		u32String convertedString = string.ConvertToStringType<char32_t>();
		wrapWidth *= 64;
		uint32 wrapWidthUint = 0;
		if (!isfinite(wrapWidth) || (wrapWidth > UINT32_MAX))
			wrapWidthUint = UINT32_MAX;
		else
			wrapWidthUint = static_cast<uint32>(wrapWidth);
		TextShaping::WrappedTextShapingContext shapingContext{ convertedString, *fontAtlasData.fontFace, wrapWidthUint };

		auto shapedText = TextShaping::ShapeTextWrapped(convertedString.FirstIterator(), convertedString.BehindIterator(), shapingContext);

		rendererTypeID = fontAtlasData.rendererTypeID;

		return CopyRenderData(shapedText, *fontAtlasData.fontFace, *fontAtlasData.atlas);
	}
	void StaticTextRenderUnit::SetColor(ColorRGBAf newColor)
	{
		color = newColor;
	}
	void StaticTextRenderUnit::SetTransform(Vec2f newPosition, Vec2f newRight)
	{
		position = newPosition;
		right = newRight;
	}
	void StaticTextRenderUnit::SetTargetFontHeight(float fontHeight)
	{
		targetFontHeight = fontHeight;
	}
	Graphics::RendererTypeID StaticTextRenderUnit::GetRequiredRendererTypeID() const
	{
		return rendererTypeID;
	}
	void StaticTextRenderUnit::Render(const Node& node, Graphics::TextRendererBase& renderer, const RenderContext& renderContext)
	{
		Vec2f up = { -right.y, right.x };
		Vec2f roundedPos = Math::Floor(position);

		float scale = targetFontHeight / atlasFontHeight;
		for (auto& glyph : glyphs)
		{
			renderer.Render({
				.texture = atlas,
				.uv1 = glyph.uv1,
				.uv2 = glyph.uv2,
				.pos = roundedPos + (right * glyph.offset.x + up * glyph.offset.y) * scale,
				.right = right * glyph.size.x * scale,
				.up = up * glyph.size.y * scale,
				.color = color,
				.blend = 1.0f,
				.alpha = 1.0f,
				}, renderContext);
		}
	}
}