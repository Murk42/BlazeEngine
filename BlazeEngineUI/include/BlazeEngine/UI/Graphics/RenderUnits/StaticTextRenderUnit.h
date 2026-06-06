#pragma once
#include "BlazeEngine/UI/Graphics/Renderers/TextRendererBase.h"
#include "BlazeEngine/UI/Graphics/RenderUnit.h"
#include <BlazeEngine/UI/Text/TextLayoutMetadata.h>
#include "BlazeEngine/UI/Text/TextStyle.h"
	
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
		Vec2f Initialize(const Text::TextLayoutMetadata& layoutMetadata, const TextStyle& style);

		ArrayView<GlyphRenderData> GetGlyphs() const { return glyphs; }
		const FontAtlas* GetAtlas() const { return atlas; }

		void Render(const Node& node, Graphics::TextRendererBase& renderer) override;
		Graphics::RendererTypeID GetRequiredRendererTypeID() const override;
	private:
		Graphics::RendererTypeID rendererTypeID = { };
		const FontAtlas* atlas = nullptr;
		Array<GlyphRenderData> glyphs;
		ColorRGBAf color;
	};
}