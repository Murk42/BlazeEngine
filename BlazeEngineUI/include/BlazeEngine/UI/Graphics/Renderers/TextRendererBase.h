#pragma once
#include "BlazeEngine/Core/Common/Handle.h"
#include "BlazeEngine/Core/Common/Color.h"
#include "BlazeEngine/Graphics/Renderers/RendererBase.h"
#include "BlazeEngine/UI/Text/FontGlyphRasterizer.h"
#include "BlazeEngine/Core/Resource/ResourceRef.h"

namespace Blaze::Graphics
{
	struct TextRectRenderData
	{
		//The RGB channels of the texture and the color are mixed using the blend parameter.
		//The A channel of the texture and the color are multiplied together with the alpha parameter.
		ResourceBaseRef texture;
		Vec2f uv1, uv2;
		Vec2f pos, right, up;
		ColorRGBAf color;
		float blend;
		float alpha;
	};

	class TextRendererBase : public RendererBase
	{
	public:
		virtual void Render(const TextRectRenderData& data, const RenderContext& context) = 0;
		virtual const UI::FontGlyphRasterizer& GetFontGlyphRasterizer() const = 0;
	};
}