#pragma once
#include "BlazeEngine/Core/Common/Rect.h"
#include "BlazeEngine/Graphics/Renderers/TexturedRectRenderer.h"
#include "BlazeEngine/UI/Graphics/RenderUnit.h"

namespace Blaze::UI::Nodes
{
	struct ImageStyle
	{
		ResourceBaseRef texture = { };
		Vec2f uv1 = Vec2f(0), uv2 = Vec2f(1);
		ColorRGBAf blendColor = ColorRGBAf(1.0f, 1.0f, 1.0f, 1.0f);
		float blendFactor = 0.0f;
		float alphaMultiplier = 1.0f;
	};

	class BLAZE_API ImageRenderUnit : public RenderUnit<Graphics::TexturedRectRenderer>
	{
	public:
		Vec2f pos, right, up;
		ImageStyle style;

		ImageRenderUnit();
		~ImageRenderUnit();

		void Render(const Node& node, Graphics::TexturedRectRenderer& renderer) override;
	};
}