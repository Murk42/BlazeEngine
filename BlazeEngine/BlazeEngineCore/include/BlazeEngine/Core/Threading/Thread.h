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

		/*
			Returns true on success, false otherwise
		*/
		template<typename F, typename ... Args> requires std::invocable<F, Args...> && SameAs<std::invoke_result_t<F, Args...>, uint>
		bool Run(F&& func, Args&& ... parameters);
		/*
			Returns true on success, false otherwise
		*/
		bool Run(BasicThreadFunction function, void* userData);

		/*
			The timeout is in seconds, returns false if the timeout period elapsed, returns true if the thread has finished.
		*/
		bool WaitToFinish(float timeout = FLT_MAX) const;
		bool IsRunning() const;
		/*
			Returns true if the thread has finished and its exit code is set, returns false if the thread is still running
		*/
		bool GetExitCode(uint& code) const;

		void SetThreadPriority(ThreadPriority newPriority);
		ThreadPriority GetThreadPriority();

		ThreadID GetThreadID() const { return id; }

		inline void* GetHandle() const { return handle; }
		void* ReleaseOwnership();

		Thread& operator=(const Thread&) = delete;
		Thread& operator=(Thread&&) noexcept;
	private:
		void* handle;
		ThreadID id;
	};

	template<typename F, typename ... Args> requires std::invocable<F, Args...> && SameAs<std::invoke_result_t<F, Args...>, uint>
	inline bool Thread::Run(F&& func, Args&& ... args)
	{
		struct ThreadArgumentData
		{
			std::atomic_flag dataCopied;
			F function;
			Tuple<Args...> argsTuple;
		};		

		auto wrapperFunction = [](void* userData) -> int {
			ThreadArgumentData* argumentData = static_cast<ThreadArgumentData*>(userData);

			Tuple<Args...> argsTuple = std::move(argumentData->argsTuple);
			F function = std::move(argumentData->function);

			argumentData->dataCopied.test_and_set();
			argumentData->dataCopied.notify_all();

			return static_cast<int>(std::apply(function, std::forward<Tuple<Args...>>(argsTuple)));
			};

		ThreadArgumentData argumentData{
			.dataCopied = ATOMIC_FLAG_INIT,
			.function = std::forward<F>(func),
			.argsTuple = { std::forward<Args>(args)... }
		};

		bool result = Run(wrapperFunction, &argumentData);

		if (!result)
			return false;

		argumentData.dataCopied.wait(false);

		return true;

	}
}