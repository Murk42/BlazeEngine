#pragma once
#include "BlazeEngineCore/Utilities/TimePoint.h"
#include "BlazeEngineCore/Utilities/TimeInterval.h"
#include "BlazeEngineCore/Debug/Logger.h"
#include <limits>

namespace Blaze
{
	constexpr TimePoint::TimePoint()
		: value(0)
	{
	}

	constexpr TimePoint::TimePoint(const TimePoint& t)
		: value(t.value)
	{
	}

	constexpr TimePoint TimePoint::operator+(const TimeInterval& other) const
	{
		return CustomAdd(*this, other);
	}

	constexpr TimePoint TimePoint::operator-(const TimeInterval& other) const
	{
		return CustomSub(*this, other);
	}

	constexpr TimeInterval TimePoint::operator-(const TimePoint& other) const
	{
		return CustomSub(*this, other);
	}

	constexpr TimePoint& TimePoint::operator+=(const TimeInterval& other)
	{
		*this = CustomAdd(*this, other);		
		return *this;
	}

	constexpr TimePoint& TimePoint::operator-=(const TimeInterval& other)
	{
		*this = CustomSub(*this, other);
		return *this;
	}

	constexpr TimePoint& TimePoint::operator=(const TimePoint& t)
	{
		value = t.value;
		return *this;
	}

	constexpr TimePoint TimePoint::Earliest()
	{
		return TimePoint(std::numeric_limits<uint64>::min());
	}

	constexpr TimePoint TimePoint::Latest()
	{
		return TimePoint(std::numeric_limits<uint64>::max());
	}

	constexpr TimePoint::TimePoint(uint64 value)
		: value(value)
	{
	}
	constexpr TimePoint TimePoint::CustomAdd(const TimePoint& a, const TimeInterval& b)
	{				
		if (b == TimeInterval::Invalid())
		{
			Debug::Logger::LogError("BlazeEngineCore", "Adding a time interval with invalid value to time point");
			return a;
		}
		else if (b == TimeInterval::Infinity())
			return Latest();
		else if (b == TimeInterval::NegativeInfinity())
			return Earliest();

		uint64 sum = static_cast<uint64>(a.value + b.value);

		if (b.value < 0 && sum > a.value)
			return Earliest();

		if (b.value > 0 && sum < a.value)
			return Latest();		

		return TimePoint(sum);
	}

	constexpr TimePoint TimePoint::CustomSub(const TimePoint& a, const TimeInterval& b)
	{		
		return CustomAdd(a, -b);
	}

	constexpr TimeInterval TimePoint::CustomSub(const TimePoint& a, const TimePoint& b)
	{
		if (a.value > b.value)
		{
			uint64 positiveSub = a.value - b.value;

			if (positiveSub > static_cast<uint64>(TimeInterval::Infinity().value))
				return TimeInterval::Infinity();
			else
				return TimeInterval(static_cast<int64>(positiveSub));
		}
		else
		{
			uint64 negativeSub = b.value - a.value;
			if (negativeSub > static_cast<uint64>(-TimeInterval::NegativeInfinity().value))
				return TimeInterval::NegativeInfinity();
			else
				return TimeInterval(-static_cast<int64>(negativeSub));
		}
	}

}