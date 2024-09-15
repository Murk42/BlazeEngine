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
				
		template<typename ... Args, typename F> requires std::invocable<F, Args...> && std::same_as<std::invoke_result_t<F, Args...>, uint>
		void Run(const F& func, Args&& ... parameters);
		//template<typename ... Args, typename F> requires std::invocable<F, Args...> && std::same_as<std::invoke_result_t<F, Args...>, uint> && std::copyable<F>
		//void Run(const F& func, Args&& ... parameters);

		//The timeout is in seconds, returns false if the timeout period elapsed, returns true if the thread has finished
		bool WaitToFinish(float timeout) const;
		bool IsRunning() const;
		//Returns true if the thread has finished and its exit code is set, returns false if the thread is still running
		bool GetExitCode(uint& code) const;

		Thread& operator=(const Thread&) = delete;
		Thread& operator=(Thread&&) noexcept;
	private:
		void* handle;

		void RunImpl(unsigned long (*function)(void*), void* task);

		template<typename F, typename ... Args> requires std::invocable<F, Args...> && std::same_as<std::invoke_result_t<F, Args...>, uint>
		struct ThreadTask
		{	
			F function;
			std::tuple<Args...> args;

			ThreadTask(const F& func, Args&& ... args) :
				function(func), args(std::forward<Args>(args)...)
			{
			}
		};

		template<typename F, typename ... Args> requires std::invocable<F, Args...> && std::same_as<std::invoke_result_t<F, Args...>, uint>
		static unsigned long ThreadFunction(void* data);
	};
	//template<typename ... Args, typename F> requires std::invocable<F, Args...> && std::same_as<std::invoke_result_t<F, Args...>, uint> && std::copyable<F>
	//inline void Thread::Run(const F& func, Args&& ... args)
	//{
	//	ThreadTask<F, Args...>* task = new ThreadTask<F, Args...>();
	//	task->function = func;
	//	task->args = std::make_tuple<Args...>(std::forward<Args>(args)...);
	//	RunImpl(&ThreadFunction<F, Args...>, task);
	//}
	template<typename ... Args, typename F> requires std::invocable<F, Args...> && std::same_as<std::invoke_result_t<F, Args...>, uint>
	inline void Thread::Run(const F& func, Args&& ... args)
	{				
		RunImpl(
			&ThreadFunction<F, Args...>, 
			new ThreadTask<F, Args...>(
				func,
				std::forward<Args>(args)...
			)
		);
	}
	template<typename F, typename ... Args> requires std::invocable<F, Args...> && std::same_as<std::invoke_result_t<F, Args...>, uint>
	inline unsigned long Thread::ThreadFunction(void* data)
	{
		ThreadTask<F, Args...>* task = (ThreadTask<F, Args...>*)data;
		
		auto copy = std::move(*task);
		delete task;
		return std::apply(copy.function, std::move(copy.args));
	}
}