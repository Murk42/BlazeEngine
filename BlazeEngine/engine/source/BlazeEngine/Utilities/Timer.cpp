#include "BlazeEngine/Utilities/Timer.h"

namespace Blaze
{	
	Timer::Timer()
		: timerStart(std::chrono::steady_clock::now())
	{		
	}
	Timer::Timer(const Timer& t)
		: timerStart(t.timerStart)
	{		
	}
	double Timer::Reset()
	{
		auto now = std::chrono::steady_clock::now();
		std::chrono::duration<double> duration = now - timerStart;
		timerStart = now;
		return duration.count();
	}
	double Timer::GetTime()
	{
		auto now = std::chrono::steady_clock::now();
		std::chrono::duration<double> duration = (now - timerStart);	
		return duration.count();
	}	
	void Timer::operator= (const Timer& t)
	{
		timerStart = t.timerStart; 
	}
}