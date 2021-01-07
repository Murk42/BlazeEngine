#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include <chrono>

namespace Blaze
{
	class BLAZE_API Timer
	{		
		std::chrono::steady_clock::time_point timerStart;		
	public:
		Timer();
		Timer(const Timer&);

		double Reset();
		double GetTime();		

		void operator= (const Timer&);
	};
}