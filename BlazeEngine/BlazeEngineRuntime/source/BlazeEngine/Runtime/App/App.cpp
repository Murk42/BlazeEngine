#include "pch.h"
#include "BlazeEngine/Runtime/App/App.h"
#include "BlazeEngine/Runtime/App/AppRuntimeThread.h"
#include "BlazeEngine/Runtime/IO/Input.h"

namespace Blaze
{
	App::App()
	{
	}
	App::~App()
	{
	}
	void App::RegisterRuntimeThread(AppRuntimeThreadCreationData&& runtimeThreadCreation)
	{
		std::lock_guard lg{ mutex };
		scheduledRuntimeThreadsForCreation.AddBack(std::move(runtimeThreadCreation));
	}
	bool App::Update()
	{
		threads.EraseAll([](const Thread& thread) -> bool { return !thread.IsRunning(); });

		ProcessScheduledRuntimeThreadsForCreation();

		return !threads.Empty();
	}
	void App::ProcessScheduledRuntimeThreadsForCreation()
	{
		std::lock_guard lg{ mutex };

		if (scheduledRuntimeThreadsForCreation.Empty())
			return;

		for (auto& runtimeThreadCreationData : scheduledRuntimeThreadsForCreation)
		{
			Thread& thread = *threads.AddFront();

			if (!runtimeThreadCreationData.InstantiateOnThread(thread))
			{
				BLAZE_LOG_ERROR("Failed to instantiate thread");
				continue;
			}

			thread.SetThreadName("AppRuntimeThread");
		}

		scheduledRuntimeThreadsForCreation.Clear();
	}
}