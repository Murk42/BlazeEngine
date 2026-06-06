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
	Vec2f StaticTextRenderUnit::Initialize(const Text::TextLayoutMetadata& layoutMetadata, const TextStyle& style)
	{	
		glyphs.Clear();

		color = style.color;

		auto fontData = style.GetFontData();

		if (fontData.fontFace == nullptr || fontData.atlas == nullptr)
		{
			rendererTypeID = 0;
			atlas = nullptr;
			return { 0, static_cast<float>(style.fontHeight) };
		}

		rendererTypeID = fontData.rendererTypeID;
		atlas = fontData.atlas;		
	
		glyphs.Resize(layoutMetadata.GetGlyphCount());

		float maxWidth = 0.0f;		
		for (uintMem i = 0; i < glyphs.Count(); ++i)
		{
			GlyphRenderData& glyphRenderData = glyphs[i];
			Text::TextLayoutMetadata::GlyphLayoutData glyphLayoutData = layoutMetadata.GetGlyphData(i);
			Text::TextLayoutMetadata::CharacterLayoutData characterLayoutData = layoutMetadata.GetCharacterData(glyphLayoutData.characterIndex);
			Text::TextLayoutMetadata::LineLayoutData lineLayoutData = layoutMetadata.GetLineData(characterLayoutData.lineIndex);

			FT_Load_Glyph(static_cast<FT_Face>(fontData.fontFace->GetHandle()), static_cast<FT_UInt>(glyphLayoutData.glyphIndex), FT_LOAD_COMPUTE_METRICS | FT_LOAD_NO_BITMAP);

			glyphRenderData.offset = Math::Floor(lineLayoutData.lineStart + glyphLayoutData.offset);
			glyphRenderData.size = Math::Floor(glyphLayoutData.size);

			Vec2u16 uv1, uv2;
			if (fontData.atlas->GetGlyphUV(glyphLayoutData.glyphIndex, uv1, uv2))
			{
				glyphRenderData.uv1 = Vec2f(uv1) / fontData.atlas->GetSize();
				glyphRenderData.uv2 = Vec2f(uv2) / fontData.atlas->GetSize();
			}
			else
			{
				glyphRenderData.uv1 = Vec2f();
				glyphRenderData.uv2 = Vec2f();
				glyphRenderData.size = Vec2f();
			}

			maxWidth = std::max(maxWidth, lineLayoutData.width);
		}		
		
		return { maxWidth, static_cast<float>(style.fontHeight) * layoutMetadata.GetLineCount() };
	}
	Graphics::RendererTypeID StaticTextRenderUnit::GetRequiredRendererTypeID() const
	{
		return rendererTypeID;
	}
	void StaticTextRenderUnit::Render(const Node& node, Graphics::TextRendererBase& renderer) 
	{
		if (atlas == nullptr || glyphs.Empty())
			return;

		auto& finalTransform = node.GetFinalTransform();
		Vec2f right = finalTransform.Right();
		Vec2f up = finalTransform.Up();
		Vec2f pos = Math::Floor(finalTransform.position);

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