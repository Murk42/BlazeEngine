#include "pch.h"
#include "BlazeEngine/UI/Graphics/RenderUnits/ImageRenderUnit.h"

namespace Blaze::UI
{
	ImageRenderUnit::ImageRenderUnit()
	{
	}
	ImageRenderUnit::~ImageRenderUnit()
	{
	}
	void ImageRenderUnit::Render(const Node& node, Graphics::TexturedRectRenderer& renderer, const RenderContext& renderContext)
	{
		renderer.Render(Graphics::TexturedRectRenderData{
			.texture = texture,
			.uv1 = uv1,
			.uv2 = uv2,
			.pos = pos,
			.right = right,
			.up = up,
			.color = color,
			.blend = blend,
			.alpha = alpha
			}, renderContext);
	}
}