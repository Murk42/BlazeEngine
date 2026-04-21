#include "pch.h"
#include "BlazeEngine/UI/Graphics/RenderUnits/QuadRenderUnit.h"

namespace Blaze::UI::Nodes
{
	void QuadRenderUnit::SetRenderData(Graphics::Quad2DRenderData newRenderData)
	{
		renderData = newRenderData;
	}
	void QuadRenderUnit::Render(const Node& node, Graphics::Quad2DRenderer& renderer) 
	{
		renderer.Render(renderData);
	}
}