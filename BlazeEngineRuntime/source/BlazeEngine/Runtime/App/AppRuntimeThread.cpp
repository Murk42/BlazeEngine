#include "pch.h"
#include "BlazeEngine/Runtime/App/AppRuntimeThread.h"
#include "BlazeEngine/Runtime/App/AppLayer.h"
#include <ranges>
#include <algorithm>

namespace Blaze
{
	thread_local AppRuntimeThread* threadAppRuntimeThreadObject = nullptr;

	static bool IsMouseMotionReportingEvent(const Input::GenericInputEvent& event)
	{
		return event.IsType<Input::MouseMotionEvent>() || event.IsType<Input::MouseEntersWindowEvent>() || event.IsType<Input::MouseLeavesWindowEvent>();
	}

	AppRuntimeThread::AppRuntimeThread()
	{
		if (Blaze::threadAppRuntimeThreadObject != nullptr)
			BLAZE_LOG_WARNING("Multiple AppRuntimeThread objects alive at the same time on the same thread");
		else
			Blaze::threadAppRuntimeThreadObject = this;
	}
	AppRuntimeThread::~AppRuntimeThread()
	{
		if (Blaze::threadAppRuntimeThreadObject == this)
			Blaze::threadAppRuntimeThreadObject = nullptr;
	}
	void AppRuntimeThread::RemoveAllLayers()
	{
		layerStack.Clear();
	}
	void AppRuntimeThread::AddLayer(AppLayerCreationData&& layerCreationData)
	{
		defferedLayerCreationData.AddBack(std::move(layerCreationData));
	}
	bool AppRuntimeThread::RemoveLayer(AppLayer& layerToBeRemoved)
	{
		for (uintMem i = 0; i < layerStack.Count(); ++i)
			if (layerStack[i].Ptr() == &layerToBeRemoved)
			{
				defferedLayerRemovalData.AddBack(i);
				return true;
			}

		return false;
	}
	void AppRuntimeThread::CreateAndRemoveDefferedLayers()
	{			
		if (!defferedLayerRemovalData.Empty())
		{
			Array<uintMem> moved = std::move(moved);

			std::sort(moved.Begin(), moved.End(), std::greater());

			for (auto& index : moved)
				layerRemovedEventDispatcher.Call({ .thread = *this, .layer = *layerStack[index] });

			for (auto& index : std::ranges::reverse_view(moved))
				layerStack.EraseAt(index);
		}

		while (!defferedLayerCreationData.Empty())
		{
			Array<AppLayerCreationData> moved = std::move(defferedLayerCreationData);

			for (auto& creationData : moved)
			{
				layerStack.AddBack(std::move(creationData.CreateLayer()));

				layerAddedEventDispatcher.Call({ .thread = *this, .layer = *layerStack.Last()});
			}
		}
	}
	Input::EventProcessedState AppRuntimeThread::SendInputEventToLayers(const Input::GenericInputEvent& inputEvent)
	{
		bool consumable = true;
		bool consumed = false;
		bool processed = false;

		Input::MouseID mouseID;
		if (IsMouseMotionReportingEvent(inputEvent))
			consumable = false;

		Input::EventProcessedState result = Input::EventProcessedState::NotProcessed;

		for (auto& layer : std::ranges::reverse_view(layerStack))
		{
			result = layer->OnEvent(inputEvent, processed);

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

		return result;
	}
}