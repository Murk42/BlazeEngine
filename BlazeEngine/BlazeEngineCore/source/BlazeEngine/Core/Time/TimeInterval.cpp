#include "pch.h"
#include "BlazeEngine/Core/Time/TimeInterval.h"
#include "BlazeEngine/Core/Time/TimePoint.h"
#include "BlazeEngine/Core/Debug/Logger.h"
#include "BlazeEngine/Core/BlazeEngineCoreContext.h"

namespace Blaze
{
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
		double ticks = seconds * blazeEngineCoreContext.tickFrequency;

		if (isnan(ticks))
			*this = Invalid();
		else if (ticks > INT64_MAX)
			*this = Infinity();
		else if (ticks <= INT64_MIN)
			*this = NegativeInfinity();
		else
			value = static_cast<int64>(ticks);

		return *this;
	}
	double TimeInterval::GetSeconds() const
	{
		if (*this == Invalid())
			return nan("");
		else if (*this == Infinity())
			return static_cast<double>(value);
		else if (*this == NegativeInfinity())
			return static_cast<double>(value);
		else
			return static_cast<double>(value) / blazeEngineCoreContext.tickFrequency;
	}
	TimeInterval& TimeInterval::SetTicks(int64 ticks)
	{
		value = ticks;
		return *this;
	}
	int64 TimeInterval::GetTicks() const
	{
		return value;
	}
	TimeInterval TimeInterval::operator-() const
	{
		TimeInterval out;
		out.value = -value;
		return out;
	}
	TimeInterval TimeInterval::operator+(const TimeInterval& other) const
	{
		return CustomAdd(*this, other);
	}
	TimeInterval TimeInterval::operator-(const TimeInterval& other) const
	{
		return CustomSub(*this, other);
	}
	TimeInterval& TimeInterval::operator+=(const TimeInterval& other)
	{
		*this = CustomAdd(*this, other);
		return *this;
	}
	TimeInterval& TimeInterval::operator-=(const TimeInterval& other)
	{
		*this = CustomSub(*this, other);
		return *this;
	}
	TimeInterval& TimeInterval::operator=(const TimeInterval& other)
	{
		value = other.value;
		return *this;
	}

	TimeInterval TimeInterval::FromSeconds(double seconds)
	{
		return TimeInterval().SetSeconds(seconds);
	}
	TimeInterval TimeInterval::FromTicks(int64 ticks)
	{
		return TimeInterval().SetTicks(ticks);
	}
	TimeInterval TimeInterval::Infinity()
	{
		return FromTicks(INT64_MAX);
	}
	TimeInterval TimeInterval::NegativeInfinity()
	{
		return FromTicks(INT64_MIN + 1);
	}
	TimeInterval TimeInterval::Zero()
	{
		return FromTicks(0);
	}
	TimeInterval TimeInterval::Invalid()
	{
		return FromTicks(INT64_MIN);
	}
	TimeInterval TimeInterval::CustomAdd(const TimeInterval& a, const TimeInterval& b)
	{
		int64 result = a.value + b.value;

		if (a == TimeInterval::Invalid() || b == TimeInterval::Invalid())
			return TimeInterval::Invalid();

		if (a.value > 0 && b.value > 0 && result < 0)
			return TimeInterval::Infinity();
		if (a.value < 0 && b.value < 0 && result > 0)
			return TimeInterval::NegativeInfinity();

		return FromTicks(result);
	}
	TimeInterval TimeInterval::CustomSub(const TimeInterval& a, const TimeInterval& b)
	{
		return CustomAdd(a, -b);
	}
}