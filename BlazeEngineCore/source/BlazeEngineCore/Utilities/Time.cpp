#include "pch.h"
#include "BlazeEngineCore/Utilities/Time.h"

namespace Blaze
{
	uint64 GetCurrentTimeValue()
	{
		//returns value in nanoseconds
		return std::chrono::high_resolution_clock::now().time_since_epoch().count();
	}	

	TimeInterval::TimeInterval()
		: value(0)
	{
	}
	TimeInterval::TimeInterval(const TimeInterval& other)
		: value(other.value)
	{
	}
	TimeInterval& TimeInterval::SetSeconds(double seconds)
	{
		value = seconds * 1e9;
		return *this;
	}
	double TimeInterval::ToSeconds() const
	{
		return (double)value * 1e-9;
	}
	TimeInterval TimeInterval::operator+(const TimeInterval& other) const
	{
		TimeInterval out;
		out.value = value + other.value;
		return out;
	}
	TimeInterval TimeInterval::operator-(const TimeInterval& other) const
	{
		TimeInterval out;
		out.value = value - other.value;
		return out;
	}
	TimeInterval& TimeInterval::operator+=(const TimeInterval& other)
	{
		value += other.value;
		return *this;
	}
	TimeInterval& TimeInterval::operator-=(const TimeInterval& other)
	{
		value -= other.value;
		return *this;
	}
	TimePoint TimeInterval::operator+(const TimePoint& other) const
	{
		return other + *this;
	}
	TimeInterval& TimeInterval::operator=(const TimeInterval& other)
	{
		value = other.value;
		return *this;
	}


	TimePoint::TimePoint()
		: value(0)
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
		time_t time = static_cast<time_t>(value / 1e10);
		localtime_s(&tm, &time);		
		size_t size = strftime(formatingBuffer, 128, format, &tm);
		return String(formatingBuffer, size);
	}

	TimePoint TimePoint::operator+(const TimeInterval& other) const
	{
		TimePoint out;
		out.value = value + other.value;
		return out;		
	}

	TimePoint TimePoint::operator-(const TimeInterval& other) const
	{
		TimePoint out;
		out.value = value - other.value;
		return out;
	}

	TimeInterval TimePoint::operator-(const TimePoint& other) const
	{		
		TimeInterval out;
		out.value = static_cast<int64>(value - other.value);
		return out;
	}

	TimePoint& TimePoint::operator+=(const TimeInterval& other)
	{
		value += other.value;
		return *this;		
	}

	TimePoint& TimePoint::operator-=(const TimeInterval& other)
	{
		value -= other.value;
		return *this;
	}

	TimePoint& TimePoint::operator=(const TimePoint& t)
	{ 
		value = t.value;
		return *this;
	}

	TimePoint TimePoint::GetCurrentWorldTime()
	{		
		TimePoint out;
		out.value = GetCurrentTimeValue();
		return out;
	}	
}