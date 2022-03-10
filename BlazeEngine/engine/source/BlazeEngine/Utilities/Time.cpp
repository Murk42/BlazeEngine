#include "BlazeEngine/Utilities/Time.h"
#include <chrono>

namespace Blaze
{
	static std::chrono::steady_clock::time_point engineStartTime = std::chrono::steady_clock::now();

	double GetCurrentTimeValue()
	{
		return double(std::chrono::high_resolution_clock::now().time_since_epoch().count()) / 1000000000.0;
	}

	//char formatingBuffer[128];

	TimePoint::TimePoint()
	{
	}

	TimePoint::TimePoint(const TimePoint& t)
		: value(t.value)
	{
	}

	String TimePoint::FormatString(const char* format)
	{
		char formatingBuffer[128];
		std::tm tm;		
		time_t time = std::chrono::system_clock::to_time_t(value);		
		localtime_s(&tm, &time);		
		size_t size = strftime(formatingBuffer, 128, format, &tm);
		return String(formatingBuffer, size);
	}

	double TimePoint::operator-(const TimePoint& t) const
	{		
		std::chrono::duration<double> diff = value - t.value;		
		return diff.count();
	}

	TimePoint& TimePoint::operator=(const TimePoint& t)
	{ 
		value = t.value;
		return *this;
	}

	TimePoint TimePoint::GetWorldTime()
	{		
		TimePoint out;
		out.value = std::chrono::system_clock::now();
		return out;
	}
	double TimePoint::GetRunTime()
	{
		std::chrono::duration<double> diff = std::chrono::steady_clock::now() - engineStartTime;
		return diff.count();
	}
}