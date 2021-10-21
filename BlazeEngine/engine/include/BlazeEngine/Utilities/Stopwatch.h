#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include <chrono>

namespace Blaze
{
	class BLAZE_API Stopwatch
	{		
		std::chrono::steady_clock::time_point timerStart;		
	public:
		Stopwatch();
		Stopwatch(const Stopwatch&);		

		double Reset();
		double GetTime();		

		void operator= (const Stopwatch&);
	};
}