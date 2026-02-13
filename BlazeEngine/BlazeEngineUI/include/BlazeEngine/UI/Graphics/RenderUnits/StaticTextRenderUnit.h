#pragma once
#include "BlazeEngine/Core/Common/Rect.h"
#include "BlazeEngine/UI/Graphics/Renderers/TextRendererBase.h"
#include "BlazeEngine/UI/Graphics/RenderUnit.h"
#include "BlazeEngine/UI/Text/TextShaping/TextShaping.h"
#include "BlazeEngine/UI/Text/FontAtlas.h"
#include "BlazeEngine/UI/Text/FontManager.h"

namespace Blaze::UI
{
	class BLAZE_API StaticTextRenderUnit : public RenderUnit<Graphics::TextRendererBase>
	{
	public:
		StaticTextRenderUnit();

		Graphics::RendererTypeID GetRequiredRendererTypeID() const override;

		void Clear();

		Vec2f BuildText(u8StringView string, const FontManager::FontAtlasData& fontAtlasData);
		Vec2f BuildWrappedText(u8StringView string, const FontManager::FontAtlasData& fontAtlasData, float wrapWidth);

		void SetColor(ColorRGBAf color);
		void SetTransform(Vec2f position, Vec2f right);
		void SetTargetFontHeight(float fontHeight);

		void Render(const Node& node, Graphics::TextRendererBase& renderer, const RenderContext& renderContext) override;

		inline float GetTargetFontHeight() const { return targetFontHeight; }
		inline float GetAtlasFontHeight() const { return atlasFontHeight; }
	private:
		struct GlyphRenderData
		{
			Blaze::Vec2f offset;
			Blaze::Vec2f size;
			Blaze::Vec2f uv1;
			Blaze::Vec2f uv2;
		};

		Graphics::RendererTypeID rendererTypeID;
		ResourceBaseRef atlas;
		float atlasFontHeight;
		Array<GlyphRenderData> glyphs;

		ColorRGBAf color;

		Vec2f position;
		Vec2f right;

		float targetFontHeight;

		Vec2f CopyRenderData(ArrayView<TextShaping::ShapedText> shapedText, const FontFace& fontFace, const FontAtlas& fontAtlas);
	};
}