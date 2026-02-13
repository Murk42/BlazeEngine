#pragma once
#include "BlazeEngine/Core/Common/Rect.h"
#include "BlazeEngine/Graphics/Renderers/TexturedRectRenderer.h"
#include "BlazeEngine/UI/Graphics/RenderUnit.h"

namespace Blaze::UI
{
	class BLAZE_API ImageRenderUnit : public RenderUnit<Graphics::TexturedRectRenderer>
	{
	public:
		Vec2f pos, right, up;
		ResourceBaseRef texture;
		Vec2f uv1 = Vec2f(0), uv2 = Vec2f(1);
		ColorRGBAf color;
		float blend = 0.0f;
		float alpha = 1.0f;

		ImageRenderUnit();
		~ImageRenderUnit();

		void Render(const Node& node, Graphics::TexturedRectRenderer& renderer, const RenderContext& renderContext) override;
	};
}