#include "pch.h"
#include "BlazeEngineCore/Threading/ThreadPool.h"

namespace Blaze
{	
	ThreadPool::ThreadPool()
		: exit(false)
	{
	}
	ThreadPool::~ThreadPool()
	{
		{
			std::lock_guard lk{ mutex };
			exit = true;
		}

		cv.notify_all();

		for (auto& thread : threads)
			thread.WaitToFinish(FLT_MAX);
	}

	void ThreadPool::AllocateThreads(uintMem threadCount)
	{		
		threads.Resize(threadCount);
		for (auto& thread : threads)
			thread.Run(ThreadFunc, (void*)this);
	}

	void ThreadPool::RunTask(TaskFunction function, void* userData)
	{
		if (function == nullptr)
			return;
		std::lock_guard lk{ mutex };
		tasks.AddBack(function, userData);

		cv.notify_one();
	}

	bool ThreadPool::IsAnyRunning()
	{
		for (auto& thread : threads)
			if (thread.IsRunning())
				return true;
		return false;
	}

	uintMem ThreadPool::WaitForAll(float timeout)
	{
		uintMem count = 0;
		for (auto& thread : threads)
			count += thread.WaitToFinish(timeout) ? 1 : 0;
		return count;
	}
	unsigned long ThreadPool::ThreadFunc(void* userData)
	{
		ThreadPool& threadPool = *(ThreadPool*)userData;

		while (true)
		{
			std::unique_lock lk{ threadPool.mutex };
			threadPool.cv.wait(lk, [&]() { return !threadPool.tasks.Empty() || threadPool.exit; });

			if (threadPool.exit)
				break;

			TaskData task = threadPool.tasks.Last();
			threadPool.tasks.EraseLast();

			lk.unlock();

			task.function(task.userData);
		}

		return 0;
	}
}