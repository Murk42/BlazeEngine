#pragma once
#include "BlazeEngineCore/Utilities/TimeInterval.h"
#include "BlazeEngineCore/Utilities/TimePoint.h"
#include "BlazeEngineCore/Debug/Logger.h"
#include <limits>

namespace Blaze
{
	constexpr TimeInterval::TimeInterval()
		: value(0)
	{
	}
	constexpr TimeInterval::TimeInterval(const TimeInterval& other)
		: value(other.value)
	{
	}
	constexpr TimeInterval& TimeInterval::SetSeconds(double seconds)
	{				
		if (seconds == std::numeric_limits<double>::quiet_NaN())
			*this = Invalid();
		else if (seconds == std::numeric_limits<double>::infinity())
			*this = Infinity();
		else if (seconds == -std::numeric_limits<double>::infinity())
			*this = NegativeInfinity();
		else
			value = static_cast<int64>(seconds * 1e9);

		return *this;
	}
	constexpr double TimeInterval::ToSeconds() const
	{		
		if (*this == Invalid())
			return std::numeric_limits<double>::quiet_NaN();
		else if (*this == Infinity())
			return std::numeric_limits<double>::infinity();
		else if (*this == NegativeInfinity())
			return -std::numeric_limits<double>::infinity();
		else
			return (double)value * 1e-9;
	}
	inline constexpr TimeInterval TimeInterval::operator-() const
	{
		return TimeInterval(-value);
	}
	constexpr TimeInterval TimeInterval::operator+(const TimeInterval& other) const
	{		
		return CustomAdd(*this, other);		
	}
	constexpr TimeInterval TimeInterval::operator-(const TimeInterval& other) const
	{
		return CustomSub(*this, other);
	}
	constexpr TimeInterval& TimeInterval::operator+=(const TimeInterval& other)
	{
		*this = CustomAdd(*this, other);
		return *this;
	}
	constexpr TimeInterval& TimeInterval::operator-=(const TimeInterval& other)
	{
		*this = CustomSub(*this, other);
		return *this;
	}	
	constexpr TimePoint TimeInterval::operator+(const TimePoint& other) const
	{
		return other.operator+(*this);
	}
	constexpr TimeInterval& TimeInterval::operator=(const TimeInterval& other)
	{
		value = other.value;
		return *this;
	}

	constexpr TimeInterval TimeInterval::FromSeconds(double seconds)
	{
		TimeInterval interval;
		interval.SetSeconds(seconds);
		return interval;
	}
	constexpr TimeInterval TimeInterval::Infinity()
	{		
		return TimeInterval(std::numeric_limits<int64>::max());
	}
	constexpr TimeInterval TimeInterval::NegativeInfinity()
	{		
		return TimeInterval(-std::numeric_limits<int64>::max());
	}	
	constexpr TimeInterval TimeInterval::Zero()
	{
		return TimeInterval(0);
	}
	inline constexpr TimeInterval TimeInterval::Invalid()
	{
		return TimeInterval(std::numeric_limits<int64>::min());
	}

	constexpr TimeInterval::TimeInterval(int64 value)
		: value(value)
	{
	}
	constexpr TimeInterval TimeInterval::CustomAdd(const TimeInterval& a, const TimeInterval& b)
	{
		int64 result = a.value + b.value;

		if (a == TimeInterval::Invalid() || b == TimeInterval::Invalid())
			return TimeInterval::Invalid();

		if (a.value > 0 && b.value > 0 && result < 0)
			return TimeInterval::Infinity();			
		if (a < 0 && b < 0 && result > 0)
			return TimeInterval::NegativeInfinity();			

		return TimeInterval(result);
	}
	constexpr TimeInterval TimeInterval::CustomSub(const TimeInterval& a, const TimeInterval& b)
	{		
		return CustomAdd(a, -b);
	}
}