#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Common.h"
#include <thread>

namespace Blaze
{
	namespace Threading
	{
		uint GetThreadID(std::thread::id id);
	}
}