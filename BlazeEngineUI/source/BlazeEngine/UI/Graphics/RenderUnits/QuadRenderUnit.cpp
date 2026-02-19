#include "BlazeEngine/UI/Graphics/RenderUnits/QuadRenderUnit.h"

namespace Blaze::UI
{
	void QuadRenderUnit::SetRenderData(Graphics::Quad2DRenderData newRenderData)
	{
		renderData = newRenderData;
	}
	void QuadRenderUnit::Render(const Node& node, Graphics::Quad2DRenderer& renderer, const RenderContext& renderContext)
	{
		renderer.Render(renderData, renderContext);
	}
}