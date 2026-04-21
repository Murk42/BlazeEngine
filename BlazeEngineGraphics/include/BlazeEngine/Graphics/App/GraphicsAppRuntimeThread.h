#pragma once
#include "BlazeEngine/Core/Event/EventMemberFunctionTie.h"
#include "BlazeEngine/Runtime/App/AppRuntimeThread.h"
#include "BlazeEngine/Runtime/IO/Window.h"
#include "BlazeEngine/Graphics/Core/GraphicsContext.h"
#include "BlazeEngine/Graphics/Core/RendererRegistry.h"

namespace Blaze
{	
	struct GraphicsAppRuntimeThreadData
	{
		Graphics::GraphicsContext& graphicsContext;
		const Graphics::RendererRegistry& rendererRegistry;
		Window& window;
	};

	class GraphicsAppRuntimeThread : public AppRuntimeThread, public GraphicsAppRuntimeThreadData
	{
	public:
		GraphicsAppRuntimeThread(GraphicsAppRuntimeThreadData data);
	private:
		void LayerAdded(const LayerAddedEvent& event);
		EVENT_MEMBER_FUNCTION(GraphicsAppRuntimeThread, LayerAdded, layerAddedEventDispatcher)
	};
}