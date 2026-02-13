#include "pch.h"
#include "BlazeEngine/Core/Time/TimePoint.h"
#include "BlazeEngine/Core/Time/TimeInterval.h"

namespace Blaze
{

	ProgramTimePoint::ProgramTimePoint()
		: value(0)
	{
		LARGE_INTEGER _value;
		QueryPerformanceCounter(&_value);
		value = _value.QuadPart;
	}
	uint64 ProgramTimePoint::GetTicks() const
	{
		return value;
	}
	ProgramTimePoint& ProgramTimePoint::SetTicks(uint64 tick)
	{
		value = tick;
		return *this;
	}
	String ProgramTimePoint::Format(StringView specifiers) const
	{
		return String();
	}
	TimeInterval ProgramTimePoint::operator-(const ProgramTimePoint& other) const
	{
		return CustomSub(*this, other);
	}
	ProgramTimePoint ProgramTimePoint::operator+(const TimeInterval& other) const
	{
		return CustomAdd(*this, other);
	}
	ProgramTimePoint ProgramTimePoint::operator-(const TimeInterval& other) const
	{
		return CustomSub(*this, other);
	}
	ProgramTimePoint& ProgramTimePoint::operator+=(const TimeInterval& other)
	{
		*this = CustomAdd(*this, other);
		return *this;
	}
	ProgramTimePoint& ProgramTimePoint::operator-=(const TimeInterval& other)
	{
		*this = CustomSub(*this, other);
		return *this;
	}
	ProgramTimePoint ProgramTimePoint::FromTicks(uint64 ticks)
	{
		return ProgramTimePoint().SetTicks(ticks);
	}
	ProgramTimePoint ProgramTimePoint::CustomAdd(const ProgramTimePoint& a, const TimeInterval& b)
	{
		if (b == TimeInterval::Invalid())
		{
			Debug::Logger::LogError("BlazeEngineCore", "Adding a time interval with invalid value to time point");
			return a;
		}
		else if (b == TimeInterval::Infinity())
			return FromTicks(UINT64_MAX);
		else if (b == TimeInterval::NegativeInfinity())
			return FromTicks(0);

		uint64 sum = static_cast<uint64>(a.value + b.GetTicks());

		if (b.GetTicks() < 0 && sum > a.value)
			return FromTicks(0);

		if (b.GetTicks() > 0 && sum < a.value)
			return FromTicks(UINT64_MAX);

		return FromTicks(sum);
	}

	ProgramTimePoint ProgramTimePoint::CustomSub(const ProgramTimePoint& a, const TimeInterval& b)
	{
		return CustomAdd(a, -b);
	}

	TimeInterval ProgramTimePoint::CustomSub(const ProgramTimePoint& a, const ProgramTimePoint& b)
	{
		if (a.value > b.value)
		{
			uint64 positiveSub = a.value - b.value;

			if (positiveSub >= static_cast<uint64>(TimeInterval::Infinity().GetTicks()))
				return TimeInterval::Infinity();
			else
				return TimeInterval::FromTicks(static_cast<int64>(positiveSub));
		}
		else
		{
			uint64 negativeSub = b.value - a.value;
			if (negativeSub > static_cast<uint64>(-TimeInterval::NegativeInfinity().GetTicks()))
				return TimeInterval::NegativeInfinity();
			else
				return TimeInterval::FromTicks(-static_cast<int64>(negativeSub));
		}
	}
}