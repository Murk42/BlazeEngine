#pragma once
#include "BlazeEngine/Graphics/Renderers/PanelRenderer.h"
#include "BlazeEngine/UI/Graphics/RenderUnit.h"

namespace Blaze::UI::Nodes
{
	struct PanelStyle
	{
		ColorRGBAf fillColor = 0x111111ff;
		ColorRGBAf borderColor = 0xeeeeeeff;
		float borderWidth = 1;
		float cornerRadius = 10;
	};

	class BLAZE_API PanelRenderUnit : public RenderUnit<Graphics::PanelRenderer>
	{
	public:
		Vec2f pos, right, up;
		PanelStyle style;		

		PanelRenderUnit();
		~PanelRenderUnit();

		void Render(const Node& node, Graphics::PanelRenderer& renderer) override;
	};
}