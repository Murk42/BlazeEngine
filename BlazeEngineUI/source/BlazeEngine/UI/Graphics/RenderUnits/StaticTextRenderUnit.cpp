#include "pch.h"
#include "BlazeEngine/UI/Graphics/RenderUnits/StaticTextRenderUnit.h"
#include "BlazeEngine/UI/Text/FontAtlas.h"
#include "BlazeEngine/UI/Text/FontFace.h"
#include <freetype/freetype.h>

namespace Blaze::UI::Nodes
{
	StaticTextRenderUnit::StaticTextRenderUnit()
	{
	}	
	void StaticTextRenderUnit::Clear()
	{
		atlas = nullptr;
		glyphs.Clear();
		rendererTypeID = { };
	}
	void StaticTextRenderUnit::SetGlyphs(Array<GlyphRenderData> glyphs, Graphics::RendererTypeID rendererTypeID, const FontAtlas* atlas)
	{
		this->glyphs = std::move(glyphs);
		this->rendererTypeID = rendererTypeID;
		this->atlas = atlas;
	}	
	void StaticTextRenderUnit::SetColor(ColorRGBAf color)
	{
		this->color = color;
	}
	Graphics::RendererTypeID StaticTextRenderUnit::GetRequiredRendererTypeID() const
	{
		return rendererTypeID;
	}
	Array<StaticTextRenderUnit::GlyphRenderData> StaticTextRenderUnit::GenerateGlyphRenderData(ArrayView<Text::ShapedString> shapedLines, const FontFace& fontFace, const FontAtlas& fontAtlas, uint32 fontHeight, Vec2f* size)
	{
		if (shapedLines.Empty())
		{
			if (size != nullptr)
				*size = { 0, static_cast<float>(fontHeight) };
			return { };
		}
		FT_Face fontFaceHandle = (FT_Face)fontFace.GetHandle();
		FT_Set_Pixel_Sizes(fontFaceHandle, static_cast<FT_UInt>(fontHeight), 0);

		uintMem glyphCount = 0;
		for (auto& shapedLine : shapedLines)
			glyphCount += shapedLine.glyphs.Count();

		Array<GlyphRenderData> glyphs{ glyphCount };

		float maxWidth = 0.0f;
		Vec2f advance{ 0, static_cast<float>(fontHeight) * (shapedLines.Count() - 1) };
		auto it = glyphs.Ptr();
		for (const auto& shapedLine : shapedLines)
		{
			advance.x = 0;

			for (const auto& glyph : shapedLine.glyphs)
			{
				FT_Load_Glyph(fontFaceHandle, glyph.glyphIndex, FT_LOAD_COMPUTE_METRICS | FT_LOAD_NO_BITMAP);

				it->offset = advance + glyph.offset + 
					Vec2f(
						static_cast<float>(fontFaceHandle->glyph->metrics.horiBearingX),
						static_cast<float>(fontFaceHandle->glyph->metrics.horiBearingY - fontFaceHandle->glyph->metrics.height)
					) / 64;
				it->size = { static_cast<float>(fontFaceHandle->glyph->metrics.width / 64) , static_cast<float>(fontFaceHandle->glyph->metrics.height / 64) };

				it->offset = Vec2f(Math::Floor(it->offset.x), Math::Floor(it->offset.y));
				it->size = Vec2f(Math::Floor(it->size.x), Math::Floor(it->size.y));

				Vec2u16 uv1, uv2;
				if (fontAtlas.GetGlyphUV(glyph.glyphIndex, uv1, uv2))
				{
					it->uv1 = Vec2f(uv1) / fontAtlas.GetSize();
					it->uv2 = Vec2f(uv2) / fontAtlas.GetSize();
				}
				else
				{
					it->uv1 = Vec2f();
					it->uv2 = Vec2f();
					it->size = Vec2f();
				}

				advance += glyph.advance;

				++it;
			}

			maxWidth = shapedLine.extent > maxWidth ? shapedLine.extent : maxWidth;
			advance.y -= static_cast<float>(fontHeight);
		}

		if (size != nullptr)
			*size = { maxWidth, (static_cast<float>(shapedLines.Count())) * fontHeight };

		return glyphs;
	}
	void StaticTextRenderUnit::Render(const Node& node, Graphics::TextRendererBase& renderer) 
	{
		if (atlas == nullptr || glyphs.Empty())
			return;

		auto& finalTransform = node.GetFinalTransform();
		Vec2f right = finalTransform.Right();
		Vec2f up = finalTransform.Up();
		Vec2f pos = Math::Floor(finalTransform.position);
		
		float atlasFontHeight = atlas->GetRasterFontHeight();

		ResourceBaseRef atlasTexture = atlas->GetTexture();
		
		for (auto& glyph : glyphs)
		{
			renderer.Render({
				.texture = atlasTexture,
				.uv1 = glyph.uv1,
				.uv2 = glyph.uv2,
				.pos = pos + (right * glyph.offset.x + up * glyph.offset.y),
				.right = right * glyph.size.x,
				.up = up * glyph.size.y,
				.color = color,
				.blend = 1.0f,
				.alpha = 1.0f,
				});
		}
	}
}