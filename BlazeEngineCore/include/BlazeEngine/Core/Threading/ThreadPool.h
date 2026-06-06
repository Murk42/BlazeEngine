#pragma once

namespace Blaze
{
	class BLAZE_API ThreadPool
	{
	public:
		using TaskFunction = void(*)(void*);

		ThreadPool();
		ThreadPool(const ThreadPool& other) = delete;
		ThreadPool(ThreadPool&& other) noexcept;
		~ThreadPool();

		void Clear();

		void SetThreadCountHint(uintMem minThreads, uintMem maxThreads);

		void RunTask(TaskFunction function, void* userData);

		ThreadPool& operator=(const ThreadPool& other) = delete;
		ThreadPool& operator=(ThreadPool&& other) noexcept;
	private:
		void* impl;	

		static int ThreadFunc(void* userData);
	};
}