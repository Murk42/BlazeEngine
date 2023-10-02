#include "pch.h"
#include "BlazeEngine/Utilities/Stopwatch.h"

namespace Blaze
{	
	Stopwatch::Stopwatch()
		: timerStart(std::chrono::steady_clock::now())
	{		
	}
	Stopwatch::Stopwatch(const Stopwatch& t)
		: timerStart(t.timerStart)
	{		
	}
	double Stopwatch::Reset()
	{
		auto now = std::chrono::steady_clock::now();
		std::chrono::duration<double> duration = now - timerStart;
		timerStart = now;
		return duration.count();
	}
	double Stopwatch::GetTime()
	{
		auto now = std::chrono::steady_clock::now();
		std::chrono::duration<double> duration = (now - timerStart);	
		return duration.count();
	}	
	void Stopwatch::operator= (const Stopwatch& t)
	{
		timerStart = t.timerStart; 
	}
}