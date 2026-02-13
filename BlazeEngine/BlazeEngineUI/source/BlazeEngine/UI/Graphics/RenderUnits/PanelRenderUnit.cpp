#include "pch.h"
#include "BlazeEngine/UI/Graphics/RenderUnits/PanelRenderUnit.h"

namespace Blaze::UI
{
	PanelRenderUnit::PanelRenderUnit()
		: pos(0), up(0), right(0)
	{
	}
	PanelRenderUnit::~PanelRenderUnit()
	{
	}
	void PanelRenderUnit::Render(const Node& node, Graphics::PanelRenderer& renderer, const RenderContext& renderContext)
 	{
		renderer.Render(Graphics::PanelRenderData{
			.pos = pos,
			.right = right,
			.up = up,
			.fillColor = fillColor,
			.borderColor = borderColor,
			.borderWidth = borderWidth,
			.cornerRadius = cornerRadius,
			}, renderContext);
	}
}