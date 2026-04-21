#pragma once
#include "BlazeEngine/Graphics/Renderers/Quad2DRenderer.h"
#include "BlazeEngine/UI/Graphics/RenderUnit.h"

namespace Blaze::UI::Nodes
{
	class BLAZE_API QuadRenderUnit : public RenderUnit<Graphics::Quad2DRenderer>
	{
	public:
		void SetRenderData(Graphics::Quad2DRenderData newRenderData);

		void Render(const Node& node, Graphics::Quad2DRenderer& renderer) override;
	private:
		Graphics::Quad2DRenderData renderData;
	};
}