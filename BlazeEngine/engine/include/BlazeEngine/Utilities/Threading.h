#pragma once
#include <thread>

namespace Blaze
{
	namespace Threading
	{
		uint GetThreadID(std::thread::id id);
	}
}