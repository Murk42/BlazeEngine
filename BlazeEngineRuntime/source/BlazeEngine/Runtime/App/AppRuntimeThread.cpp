#include "pch.h"
#include "BlazeEngine/Runtime/App/AppRuntimeThread.h"
#include "BlazeEngine/Runtime/IO/Window.h"
#include <ranges>

namespace Blaze
{
	AppRuntimeThread::AppRuntimeThread(Window& window)
		: window(window), shouldExit(false)
	{
	}
	void AppRuntimeThread::MarkForExit()
	{
		shouldExit = true;
	}
	void AppRuntimeThread::AddLayer(AppLayerCreationData&& layerCreationData)
	{
		std::lock_guard lg{ layerTasksMutex };
		queuedLayerCreationData.AddBack(std::move(layerCreationData));
	}
	void AppRuntimeThread::RemoveLayer(AppLayer& layer)
	{
		LayerRemovalData removalData{
			.pickFunction = [](AppLayer& layer, void* userData) { return &layer == static_cast<AppLayer*>(userData); },
			.userData = &layer
		};
		RemoveLayer(removalData);
	}
	void AppRuntimeThread::Run()
	{
		while (!shouldExit)
		{
			ProcessInputEvents();

			for (auto& layer : layerStack)
				layer->Update();

			StartRender();

			for (auto& layer : layerStack)
				layer->Render();

			EndRender();

			ProcessLayerTasks();
		}

		layerStack.Clear();
	}
	static bool GetMouseMotionReportingEventMouseID(Input::GenericInputEvent& event, Input::MouseID& mouseID)
	{		
		if (event.TryProcess([&](const Input::MouseMotionEvent      & event) { mouseID = event.mouseID; })) return true;
		if (event.TryProcess([&](const Input::MouseEntersWindowEvent& event) { mouseID = event.mouseID; })) return true;
		if (event.TryProcess([&](const Input::MouseLeavesWindowEvent& event) { mouseID = event.mouseID; })) return true;
		return false;
	}
	void AppRuntimeThread::ProcessInputEvents()
	{
		Input::GenericInputEvent inputEvent;
		while (window.ProcessInputEvent(inputEvent))
		{
			bool consumable = true;
			bool consumed = false;
			bool processed = false;

			Input::MouseID mouseID;
			if (GetMouseMotionReportingEventMouseID(inputEvent, mouseID))
			{
				consumable = false;

				//if (window.IsMouseCaptured(mouseID))
				//	processed = true;
			}

			for (auto& layer : std::ranges::reverse_view(layerStack))
			{
				Input::EventProcessedState result = layer->OnEvent(inputEvent, processed);

				switch (result)
				{
				case Input::EventProcessedState::NotProcessed:
					break;
				case Input::EventProcessedState::Processed:
					processed = true;
					break;
				case Input::EventProcessedState::Consumed:
					if (consumable)
						consumed = true;
					processed = true;
					break;
				default:
					BLAZE_LOG_ERROR("Client function returned an invalid EventProcessedState enum value");
					break;
				}

				if (consumed)
					break;
			}
		}
	}
	void AppRuntimeThread::ProcessLayerTasks()
	{
		Array<LayerRemovalData> movedQueuedLayerRemovalData;
		Array<AppLayerCreationData> movedQueuedLayerCreationData;

		{
			std::lock_guard lg{ layerTasksMutex };
			movedQueuedLayerRemovalData = std::move(queuedLayerRemovalData);
			movedQueuedLayerCreationData = std::move(queuedLayerCreationData);
		}

		for (auto& removalData : movedQueuedLayerRemovalData)
			for (auto it = layerStack.FirstIterator(); it != layerStack.BehindIterator(); ++it)
				if (removalData.pickFunction(**it, removalData.userData))
				{
					layerStack.EraseAt(it);
					break;
				}

		for (auto& creationData : movedQueuedLayerCreationData)
			layerStack.AddBack(creationData.CreateLayer());
	}
	void AppRuntimeThread::RemoveLayer(const LayerRemovalData& layerRemovalData)
	{
		if (layerRemovalData.pickFunction == nullptr)
			return;

		std::lock_guard lg{ layerTasksMutex };
		queuedLayerRemovalData.AddBack(layerRemovalData);
	}
}