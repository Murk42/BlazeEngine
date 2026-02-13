#pragma once
#include "BlazeEngine/Core/Container/Array.h"
#include "BlazeEngine/Core/Threading/Thread.h"
#include <mutex>

namespace Blaze
{
	class BLAZE_API ThreadPool
	{
	public:
		using TaskFunction = void(*)(void*);

		ThreadPool();
		~ThreadPool();

		void AllocateThreads(uintMem threads);

		void RunTask(TaskFunction function, void* userData);

		bool IsAnyRunning();
		uintMem WaitForAll(float timeout);

		inline uintMem ThreadCount() const { return threads.Count(); }
	private:
		struct TaskData
		{
			TaskFunction function;
			void* userData;
		};

		bool exit : 1;
		Array<Blaze::Thread> threads;
		Array<TaskData> tasks;
		std::condition_variable cv;
		std::mutex mutex;

		static int ThreadFunc(void* userData);
	};
}