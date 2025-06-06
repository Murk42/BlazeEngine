#pragma once
#include "BlazeEngineCore/Threading/Thread.h"
#include "BlazeEngineCore/DataStructures/Array.h"

namespace Blaze
{
	class BLAZE_CORE_API ThreadGroup
	{
	public:
		ThreadGroup();

		void AllocateThreads(uintMem threads);

		/*
			Runs on each thred in the thread group the task with the first parameter being the thread index and
			the second the thread count
		*/
		template<typename ... Args, std::invocable<uintMem, uintMem, Args...> F>
		void RunTask(const F& task, Args&& ... args);

		bool IsAnyRunning();
		uintMem WaitForAll(float timeout);

		inline uintMem ThreadCount() const { return threads.Count(); }
	private:
		Array<Blaze::Thread> threads;
	};

	template<typename ...Args, std::invocable<uintMem, uintMem, Args...> F>
	inline void ThreadGroup::RunTask(const F& task, Args && ...args)
	{
		if (threads.Empty())
			task(0, 1, std::forward<Args>(args)...);
		else
			for (uintMem i = 0; i < threads.Count(); ++i)
				threads[i].Run(task, (uintMem)i, threads.Count(), std::forward<Args>(args)...);
	}
}