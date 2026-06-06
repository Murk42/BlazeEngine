#include "pch.h"
#ifndef BLAZE_PLATFORM_WINDOWS
#include "BlazeEngine/Core/Threading/ThreadPool.h"
#include "BlazeEngine/Core/Container/Array.h"
#include "BlazeEngine/Core/Threading/Thread.h"
#include <mutex>
#include <condition_variable>

namespace Blaze
{
	struct TaskData
	{
		ThreadPool::TaskFunction function;
		void* userData;
	};

	struct ThreadPoolImpl
	{
		bool exit : 1 = false;
		Array<Blaze::Thread> threads;
		Array<TaskData> tasks;
		std::condition_variable cv;
		std::mutex mutex;				
	};
#undef THIS
#define THIS static_cast<ThreadPoolImpl*>(this->impl)

	ThreadPool::ThreadPool()
		: impl(nullptr)
	{
	}
	ThreadPool::ThreadPool(ThreadPool&& other) noexcept
		: impl(std::exchange(other.impl, nullptr))
	{
	}
	ThreadPool::~ThreadPool()
	{
		Clear();
	}

	void ThreadPool::Clear()
	{
		if (impl == nullptr)
			return;

		{
			std::lock_guard lk{ THIS->mutex };
			THIS->exit = true;
		}

		THIS->cv.notify_all();

		for (auto& thread : THIS->threads)
			thread.WaitToFinish(FLT_MAX);

		delete impl;
	}
	void ThreadPool::SetThreadCountHint(uintMem minThreads, uintMem maxThreads)
	{
		if (impl == nullptr) impl = new ThreadPoolImpl();

		THIS->threads.Resize((minThreads + maxThreads + 1) / 2);

		for (auto& thread : THIS->threads)
			thread.Run(ThreadFunc, (void*)this);
	}
	void ThreadPool::RunTask(TaskFunction function, void* userData)
	{
		if (impl == nullptr) impl = new ThreadPoolImpl();

		if (function == nullptr)
			return;

		std::lock_guard lk{ THIS->mutex };
		THIS->tasks.AddBack(function, userData);

		THIS->cv.notify_one();
	}
	ThreadPool& ThreadPool::operator=(ThreadPool&& other) noexcept
	{
		Clear();

		impl = std::exchange(other.impl, nullptr);

		return *this;
	}
	int ThreadPool::ThreadFunc(void* userData)
	{
		ThreadPoolImpl& threadPool = *static_cast<ThreadPoolImpl*>(static_cast<ThreadPool*>(userData)->impl);

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
#endif