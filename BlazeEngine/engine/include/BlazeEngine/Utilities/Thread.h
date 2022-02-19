#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include <functional>

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

		void Run(std::function<int()>);
		void WaitToFinish();
		void Terminate();

		Thread& operator=(const Thread&) = delete;
		Thread& operator=(Thread&&) noexcept;
	};
}