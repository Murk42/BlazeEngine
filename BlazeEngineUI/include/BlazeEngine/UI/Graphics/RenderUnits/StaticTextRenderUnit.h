#pragma once
#include "BlazeEngine/Core/Common/Rect.h"
#include "BlazeEngine/Graphics/Renderers/TexturedRectRenderer.h"
#include "BlazeEngine/UI/Graphics/RenderUnit.h"
#include "BlazeEngine/UI/Text/TextShaping/TextShaping.h"
#include "BlazeEngine/UI/Text/FontAtlas.h"

namespace Blaze::UI
{
	class BLAZE_API StaticTextRenderUnit : public RenderUnit<Graphics::TexturedRectRenderer>
	{
	public:
		struct GlyphRenderData
		{
			Blaze::Vec2f offset;
			Blaze::Vec2f size;
			Blaze::Vec2f uv1;
			Blaze::Vec2f uv2;
		};

		ColorRGBAf color;
		Vec2f position;
		Vec2f right;
		ResourceBaseRef atlas;
		Array<GlyphRenderData> glyphs;

		StaticTextRenderUnit();

		void Clear();

		Vec2f CopyRenderData(ArrayView<TextShaping::ShapedText> shapedText, float fontSize, const FontAtlas& atlas);
		Vec2f BuildText(u8StringView string, float fontSize, const FontAtlas& atlas);
		Vec2f BuildWrappedText(u8StringView string, float fontSize, const FontAtlas& atlas, float wrapWidth);

		void Render(const Node& node, Graphics::TexturedRectRenderer& renderer, const RenderContext& renderContext) override;
	};
}