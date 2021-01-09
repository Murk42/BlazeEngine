#include "BlazeEngine/Utilities/Time.h"
#include <chrono>

#include "Engine.h"

namespace Blaze
{ 
	namespace Time
	{		
		inline double GetCurrentTimeValue()
		{
			return double(std::chrono::high_resolution_clock::now().time_since_epoch().count()) / 1000000000.0;
		}

		double GetWorldTime()
		{					
			auto now = std::chrono::system_clock::now();
			std::chrono::duration<double> diff = now.time_since_epoch();
			return diff.count();
		}
		double GetRunTime()
		{
			std::chrono::duration<double> diff = std::chrono::steady_clock::now() - engine->Time.engineStartTime;
			return diff.count();
		}
	}
}