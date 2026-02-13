#pragma once
#include "BlazeEngine/Core/Container/DualList.h"
#include "BlazeEngine/Runtime/App/AppRuntimeThreadCreationData.h"

namespace Blaze
{
	class App
	{
	public:
		App();
		~App();
		
		void RegisterRuntimeThread(AppRuntimeThreadCreationData&& runtimeThreadCreation);

		bool Update();
	private:
		DualList<Thread> threads;
		Array<AppRuntimeThreadCreationData> scheduledRuntimeThreadsForCreation;

		std::recursive_mutex mutex;

		void ProcessScheduledRuntimeThreadsForCreation();
	};
}