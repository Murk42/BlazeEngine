#include "pch.h"
#include "BlazeEngine/Graphics/Renderers/Quad2DRenderer.h"

namespace Blaze::Graphics
{
	void Quad2DRenderer::Render(Vec2f pos, Vec2f size, float rotation, ColorRGBAf color, const RenderContext& context)
	{
		float sin = Math::Sin(rotation);
		float cos = Math::Cos(rotation);
		Render(Quad2DRenderData {
			pos,
			pos + size * Vec2f(cos, sin),
			pos + size * Vec2f(-sin, cos),
			color
			}, context);
	}
	void Quad2DRenderer::Render(Vec2f pos, Vec2f size, Vec2f right, ColorRGBAf color, const RenderContext& context)
	{
		right.Normalise();
		Render(Quad2DRenderData{
			pos,
			pos + right * size.x,
			pos + Vec2f(-right.y, right.x) * size.y,
			color
			}, context);
	}
}