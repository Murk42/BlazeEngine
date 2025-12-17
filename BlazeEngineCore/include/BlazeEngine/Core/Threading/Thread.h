#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include <atomic>

namespace Blaze
{
	enum class ThreadPriority
	{
		Idle,
		Lowest,
		Low,
		Normal,
		High,
		Highest,
		TimeCritical
	};

	class BLAZE_API Thread
	{
	public:
		using BasicThreadFunction = int(*)(void*);
		using ThreadID = uint32;

		Thread();
		Thread(const Thread&) = delete;
		Thread(Thread&&) noexcept;
		~Thread();

		inline bool Empty() const { return handle == nullptr; }
		void Release();

		void SetThreadName(u8StringView name);
		u8String GetThreadName() const;

		template<typename ... Args, typename F> requires std::invocable<F, Args...> && SameAs<std::invoke_result_t<F, Args...>, uint>
		ThreadID Run(F&& func, Args&& ... parameters);
		ThreadID Run(BasicThreadFunction function, void* userData);

		/*
			The timeout is in seconds, returns false if the timeout period elapsed, returns true if the thread has finished.
		*/
		bool WaitToFinish(float timeout = FLT_MAX) const;
		bool IsRunning() const;
		//Returns true if the thread has finished and its exit code is set, returns false if the thread is still running
		bool GetExitCode(uint& code) const;

		void SetThreadPriority(ThreadPriority newPriority);
		ThreadPriority GetThreadPriority();

		inline void* GetHandle() const { return handle; }
		void* ReleaseOwnership();

		Thread& operator=(const Thread&) = delete;
		Thread& operator=(Thread&&) noexcept;
	private:
		void* handle;
	};

	template<typename ... Args, typename F> requires std::invocable<F, Args...> && SameAs<std::invoke_result_t<F, Args...>, uint>
	inline Thread::ThreadID Thread::Run(F&& func, Args&& ... args)
	{
		struct ThreadArgumentData
		{
			std::atomic_flag occupied;

			void* arguments;
			void* function;
		};

		//thread_local is used to eliminate the risk of multiple threads waiting for each other to run a thread
		static thread_local ThreadArgumentData argumentData{
			.occupied = ATOMIC_FLAG_INIT,
			.arguments = nullptr,
			.function = nullptr
		};

		Tuple<Args...> argsTuple{ std::forward<Args>(args)... };

		argumentData.occupied.wait(true);
		argumentData.occupied.test_and_set();
		argumentData.arguments = &argsTuple;
		argumentData.function = &func;

		auto function = [](void* userData) -> int {
			ThreadArgumentData* argumentData = static_cast<ThreadArgumentData*>(userData);

			Tuple<Args...> argsTuple = std::move(*static_cast<Tuple<Args...>*>(argumentData->arguments));
			F function = std::move(*static_cast<F*>(argumentData->function));

			argumentData->occupied.clear();
			argumentData->occupied.notify_all();

			std::apply(function, argsTuple);
			};

		return Run(function, &argumentData);
	}
}