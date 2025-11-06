#pragma once
#include <mutex>

namespace Blaze
{	
	class ThreadTaskQueue
	{		
		std::mutex waitingMutex;

		std::function<void()> task;				

		std::atomic_flag activeFlag;
	public:
		void EnqueueTask(std::function<void()>&& task)
		{
			std::lock_guard<std::mutex> waiting_LG{ waitingMutex };

			this->task = std::move(task);

			activeFlag.test_and_set();
			activeFlag.notify_one();

			activeFlag.wait(1);
		}
		
		void CompleteTask()
		{
			if (!activeFlag.test())
				return;

			task();

			activeFlag.clear();
			activeFlag.notify_one();
		}
	};	
}