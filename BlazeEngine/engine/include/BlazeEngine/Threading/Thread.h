#pragma once

namespace Blaze
{
	class Thread
	{
		void* handle;
	public:
		Thread();
		Thread(std::function<int()>);
		Thread(const Thread&) = delete;
		Thread(Thread&&) noexcept;
		~Thread();

		Result Run(std::function<int()>);
		Result Run(int(*func)());

		void WaitToFinish() const;				 
		bool IsRunning() const;

		Thread& operator=(const Thread&) = delete;
		Thread& operator=(Thread&&) noexcept;
	};
}