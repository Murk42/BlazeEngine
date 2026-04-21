#include "pch.h"
#include "BlazeEngine/UI/Graphics/RenderUnits/ImageRenderUnit.h"

namespace Blaze::UI::Nodes
{
	ImageRenderUnit::ImageRenderUnit()
	{
	}
	ImageRenderUnit::~ImageRenderUnit()
	{
	}
	void ImageRenderUnit::Render(const Node& node, Graphics::TexturedRectRenderer& renderer) 
	{
		renderer.Render(Graphics::TexturedRectRenderData{
			.texture = style.texture,
			.uv1 = style.uv1,
			.uv2 = style.uv2,
			.pos = pos,
			.right = right,
			.up = up,
			.color = style.blendColor,
			.blend = style.blendFactor,
			.alpha = style.alphaMultiplier
			});
	}
}