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
	void App::Start()
	{
		while (true)
		{
			ProcessScheduledRuntimeThreadsForCreation();				

			if (threads.Empty())
				break;

			Input::WaitForEvents(0.100f);

			while (threadExitedCount.load() != 0)
			{
				threads.EraseAll([&](const Thread& thread) -> bool
					{
						if (!thread.IsRunning())
						{
							--threadExitedCount;
							return true;
						}
						return false;
					});
			}
		}
	}
	void App::ProcessScheduledRuntimeThreadsForCreation()
	{
		std::lock_guard lg{ mutex };

		if (scheduledRuntimeThreadsForCreation.Empty())
			return;

		for (auto& runtimeThreadCreationData : scheduledRuntimeThreadsForCreation)
		{
			Thread& thread = *threads.AddFront();

			if (!thread.Run([&](AppRuntimeThreadCreationData creationData) -> int
				{
					creationData.Run([](AppRuntimeThread& thread)
						{
							thread.CreateAndRemoveDefferedLayers();

							thread.Run();

							thread.RemoveAllLayers();
						});

					auto context = BlazeEngineContext::GetEngineContext();
					if (context != nullptr)
						context->NudgeMainThread();

					++threadExitedCount;

					return 0;
				}, AppRuntimeThreadCreationData(std::move(runtimeThreadCreationData))))
			{
				BLAZE_LOG_ERROR("Failed to instantiate thread");
				continue;
			}

			thread.SetThreadName("AppRuntimeThread");
		}

		scheduledRuntimeThreadsForCreation.Clear();
	}
}