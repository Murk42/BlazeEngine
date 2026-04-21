#include "pch.h"
#include "BlazeEngine/Graphics/App/GraphicsAppRuntimeThread.h"
#include "BlazeEngine/Graphics/App/GraphicsAppLayer.h"

namespace Blaze
{
	GraphicsAppRuntimeThread::GraphicsAppRuntimeThread(GraphicsAppRuntimeThreadData data)
		: GraphicsAppRuntimeThreadData(data)
	{

	}
	void GraphicsAppRuntimeThread::LayerAdded(const LayerAddedEvent& event)
	{
		if (auto* graphicsLayer = dynamic_cast<GraphicsAppLayer*>(&event.layer))
			graphicsLayer->SetupGraphics(*this);
	}
}
