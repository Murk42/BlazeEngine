#include "pch.h"
#include "BlazeEngineCore/Utilities/TimePoint.h"

namespace Blaze
{	
	
	String TimePoint::FormatString(const char* format)
	{
		char formatingBuffer[128];
		std::tm tm;		
		time_t time = static_cast<time_t>(value / 1e10);
		localtime_s(&tm, &time);		
		size_t size = strftime(formatingBuffer, 128, format, &tm);
		return String(formatingBuffer, size);
	}

	static uint64 GetCurrentTimeValue()
	{
		//returns value in nanoseconds
		return std::chrono::high_resolution_clock::now().time_since_epoch().count();
	}		
	
	TimePoint TimePoint::GetCurrentWorldTime()
	{		
		TimePoint out;
		out.value = GetCurrentTimeValue();
		return out;
	}
}