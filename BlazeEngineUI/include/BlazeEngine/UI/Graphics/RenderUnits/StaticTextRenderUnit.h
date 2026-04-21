#pragma once
#include "BlazeEngine/UI/Graphics/Renderers/TextRendererBase.h"
#include "BlazeEngine/UI/Graphics/RenderUnit.h"
#include <BlazeEngine/UI/Text/TextShaper.h>
#include "BlazeEngine/UI/Text/FontAtlas.h"
#include "BlazeEngine/UI/Text/FontManager.h"

namespace Blaze::UI::Nodes
{
	class BLAZE_API StaticTextRenderUnit : public RenderUnit<Graphics::TextRendererBase>
	{
	public:
		struct GlyphRenderData
		{
			Vec2f offset;
			Vec2f size;
			Vec2f uv1;
			Vec2f uv2;
		};

		StaticTextRenderUnit();
		
		void Clear();
		void SetGlyphs(Array<GlyphRenderData> glyphs, Graphics::RendererTypeID rendererTypeID, const FontAtlas* atlas);
		void SetColor(ColorRGBAf color);

		ArrayView<GlyphRenderData> GetGlyphs() const { return glyphs; }
		const FontAtlas* GetAtlas() const { return atlas; }

		void Render(const Node& node, Graphics::TextRendererBase& renderer) override;
		Graphics::RendererTypeID GetRequiredRendererTypeID() const override;

		static Array<GlyphRenderData> GenerateGlyphRenderData(ArrayView<Text::ShapedString> shapedLines, const FontFace& fontFace, const FontAtlas& fontAtlas, uint32 fontHeight, Vec2f* size);
	private:
		Graphics::RendererTypeID rendererTypeID = { };
		const FontAtlas* atlas = nullptr;
		Array<GlyphRenderData> glyphs;
		ColorRGBAf color;
	};
}