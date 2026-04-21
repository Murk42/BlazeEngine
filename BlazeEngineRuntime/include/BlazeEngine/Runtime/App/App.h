#pragma once
#include "BlazeEngine/Core/Container/DualList.h"
#include "BlazeEngine/Runtime/App/AppRuntimeThreadCreationData.h"
#include "BlazeEngine/Core/Threading/Thread.h"

namespace Blaze
{
	class App
	{
	public:
		App();
		~App();
		
		void RegisterRuntimeThread(AppRuntimeThreadCreationData&& runtimeThreadCreation);

		void Start();
	private:
		DualList<Thread> threads;
		Array<AppRuntimeThreadCreationData> scheduledRuntimeThreadsForCreation;
		std::atomic_uint32_t threadExitedCount;

		std::recursive_mutex mutex;

		void ProcessScheduledRuntimeThreadsForCreation();
	};
}