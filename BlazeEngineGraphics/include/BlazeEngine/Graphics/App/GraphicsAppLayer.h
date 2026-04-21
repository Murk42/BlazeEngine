#pragma once
#include "BlazeEngine/Runtime/App/AppLayer.h"
#include "BlazeEngine/Graphics/App/GraphicsAppRuntimeThread.h"

namespace Blaze
{
	class GraphicsAppLayer : public virtual AppLayer
	{
	public:
		virtual void SetupGraphics(const GraphicsAppRuntimeThreadData& data) = 0;
		virtual void Render(const Graphics::RenderContext& renderContext) = 0;
	};
}