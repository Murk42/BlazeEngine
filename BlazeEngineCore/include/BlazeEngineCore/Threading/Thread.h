#pragma once

namespace Blaze
{
	class BLAZE_CORE_API Thread
	{
	public:
		Thread();		
		Thread(const Thread&) = delete;
		Thread(Thread&&) noexcept;
		~Thread();
				
		template<typename F, typename ... Args> requires std::invocable<F, Args...> && std::same_as<uint, std::invoke_result_t<F, Args...>>
		Result Run(F func, Args&& ... parameters);

		//The timeout is in seconds, returns false if the timeout period elapsed, returns true if the thread has finished
		bool WaitToFinish(float timeout) const;
		bool IsRunning() const;
		//Returns true if the thread has finished and its exit code is set, returns false if the thread is still running
		bool GetExitCode(uint& code) const;

		Thread& operator=(const Thread&) = delete;
		Thread& operator=(Thread&&) noexcept;
	private:
		void* handle;

		Result Run(unsigned long (*function)(void*), void* task);

		template<typename F, typename ... Args> requires std::invocable<F, Args...>&& std::same_as<uint, std::invoke_result_t<F, Args...>>
		struct ThreadTask
		{	
			F function;
			std::tuple<Args...> args;
		};

		template<typename F, typename ... Args> requires std::invocable<F, Args...>&& std::same_as<uint, std::invoke_result_t<F, Args...>>
		static unsigned long ThreadFunction(void* data);
	};
	template<typename F, typename ...Args> requires std::invocable<F, Args...>&& std::same_as<uint, std::invoke_result_t<F, Args...>>
	inline Result Thread::Run(F func, Args&& ... args)
	{
		ThreadTask<F, Args...>* task = new ThreadTask<F, Args...>();
		task->function = func;
		task->args = std::make_tuple<Args...>(std::forward<Args>(args)...);
		return Run(&ThreadFunction<F, Args...>, task);
	}
	template<typename F, typename ...Args> requires std::invocable<F, Args...>&& std::same_as<uint, std::invoke_result_t<F, Args...>>
	inline unsigned long Thread::ThreadFunction(void* data)
	{
		ThreadTask<F, Args...>* task = (ThreadTask<F, Args...>*)data;
		auto copy = std::move(*task);
		delete task;
		return static_cast<unsigned long>(std::apply(copy.function, std::move(copy.args)));
	}
}