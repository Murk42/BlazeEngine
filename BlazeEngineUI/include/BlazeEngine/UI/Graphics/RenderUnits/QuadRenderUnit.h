#pragma once
#include "BlazeEngine/Graphics/Renderers/Quad2DRenderer.h"
#include "BlazeEngine/UI/Graphics/RenderUnit.h"

namespace Blaze::UI
{
	class BLAZE_API QuadRenderUnit : public RenderUnit<Graphics::Quad2DRenderer>
	{
	public:
		void SetRenderData(Graphics::Quad2DRenderData newRenderData);

		void Render(const Node& node, Graphics::Quad2DRenderer& renderer, const RenderContext& renderContext) override;
	private:
		Graphics::Quad2DRenderData renderData;
	};
}