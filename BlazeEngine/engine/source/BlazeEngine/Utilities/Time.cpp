#include "BlazeEngine/Utilities/Time.h"
#include <chrono>

#include "source/BlazeEngine/Internal/Engine.h"

namespace Blaze
{
	double GetCurrentTimeValue()
	{
		return double(std::chrono::high_resolution_clock::now().time_since_epoch().count()) / 1000000000.0;
	}

	Time::Time(double seconds)
		: value(seconds)
	{		
	}

	char formatingBuffer[128];

	String Time::FormatString(const char* format)
	{
		time_t time = value;
		tm tm;
		localtime_s(&tm, &time);		
		size_t size = strftime(formatingBuffer, 128, format, &tm);
		return String(formatingBuffer, size);
	}

	Time Time::GetWorldTime()
	{
		auto now = std::chrono::system_clock::now();
		std::chrono::duration<double> diff = now.time_since_epoch();
		return Time(diff.count());
	}
	Time Time::GetRunTime()
	{
		std::chrono::duration<double> diff = std::chrono::steady_clock::now() - engine->Time.engineStartTime;
		return Time(diff.count());
	}
}