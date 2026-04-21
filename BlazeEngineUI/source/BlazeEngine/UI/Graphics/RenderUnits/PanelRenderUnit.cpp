#include "pch.h"
#include "BlazeEngine/UI/Graphics/RenderUnits/PanelRenderUnit.h"

namespace Blaze::UI::Nodes
{
	PanelRenderUnit::PanelRenderUnit()
		: pos(0), up(0), right(0), style()
	{
	}
	PanelRenderUnit::~PanelRenderUnit()
	{
	}
	void PanelRenderUnit::Render(const Node& node, Graphics::PanelRenderer& renderer) 
 	{
		renderer.Render(Graphics::PanelRenderData{
			.pos = pos,
			.right = right,
			.up = up,
			.fillColor = style.fillColor,
			.borderColor = style.borderColor,
			.borderWidth = style.borderWidth,
			.cornerRadius = style.cornerRadius,
			});
	}
}