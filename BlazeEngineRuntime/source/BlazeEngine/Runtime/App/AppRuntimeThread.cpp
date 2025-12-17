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
		std::lock_guard lg{ layerMutex };
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

			ProcessMessages();

			ProcessLayerTasks();
		}

		layerStack.Clear();
	}
	void AppRuntimeThread::ProcessInputEvents()
	{
		Input::GenericInputEvent inputEvent;
		while (window.ProcessInputEvent(inputEvent))
		{
			bool processed = false;

			for (auto& layer : std::ranges::reverse_view(layerStack))
			{
				processed |= layer->OnEvent(inputEvent, processed);

				bool eventBlocked = false;

				switch (inputEvent.GetValueType())
				{
				case Input::GenericInputEvent::GetValueTypeOf<Input::MouseMotionEvent>():
				case Input::GenericInputEvent::GetValueTypeOf<Input::MouseButtonDownEvent>():
				case Input::GenericInputEvent::GetValueTypeOf<Input::MouseButtonUpEvent>():
				case Input::GenericInputEvent::GetValueTypeOf<Input::MouseScrollEvent>():
					eventBlocked = false;
					break;
				default:
					eventBlocked = true;
					break;
				}

				if (eventBlocked)
					break;
			}
		}
	}
	void AppRuntimeThread::ProcessMessages()
	{
		Array<Handle<Message>> movedMessages;
		{
			std::lock_guard lg{ messagesMutex };
			movedMessages = std::move(messages);
		}

		for (auto& message : movedMessages)
			ProcessMessage(*message);
	}
	void AppRuntimeThread::ProcessLayerTasks()
	{
		std::lock_guard lg{ layerMutex };
		Array<LayerRemovalData> movedQueuedLayerRemovalData = std::move(queuedLayerRemovalData);
		Array<AppLayerCreationData> movedQueuedLayerCreationData = std::move(queuedLayerCreationData);

		for (auto& removalData : movedQueuedLayerRemovalData)
			for (auto it = layerStack.FirstIterator(); it != layerStack.BehindIterator(); ++it)
				if (removalData.pickFunction(**it, removalData.userData))
				{
					layerStack.EraseAt(it);
					break;
				}

		for (auto& creationData : movedQueuedLayerCreationData)
			creationData.CreateLayer(layerStack);
	}
	void AppRuntimeThread::SendMessage(Handle<Message> message)
	{
		if (message == nullptr)
			return;

		std::lock_guard lg{ messagesMutex };
		messages.AddBack(std::move(message));
	}
	void AppRuntimeThread::RemoveLayer(const LayerRemovalData& layerRemovalData)
	{
		if (layerRemovalData.pickFunction == nullptr)
			return;


		std::lock_guard lg{ layerMutex };
		queuedLayerRemovalData.AddBack(layerRemovalData);
	}
}