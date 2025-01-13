#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"

namespace Blaze
{
	class TimePoint;

	class BLAZE_CORE_API TimeInterval
	{
	public:
		constexpr TimeInterval();
		constexpr TimeInterval(const TimeInterval& other);

		constexpr TimeInterval& SetSeconds(double seconds);
		constexpr double ToSeconds() const;

		constexpr TimeInterval operator-() const;
		constexpr TimeInterval operator+(const TimeInterval&) const;
		constexpr TimeInterval operator-(const TimeInterval&) const;
		constexpr TimeInterval& operator+=(const TimeInterval&);
		constexpr TimeInterval& operator-=(const TimeInterval&);
		constexpr TimePoint operator+(const TimePoint&) const;
		constexpr inline bool operator<(const TimeInterval& other) const { return value < other.value; }
		constexpr inline bool operator>(const TimeInterval& other) const { return value > other.value; }
		constexpr inline bool operator<=(const TimeInterval& other) const { return value <= other.value; }
		constexpr inline bool operator>=(const TimeInterval& other) const { return value >= other.value; }
		constexpr inline bool operator==(const TimeInterval& other) const { return value == other.value; }
		constexpr inline bool operator!=(const TimeInterval& other) const { return value != other.value; }

		constexpr TimeInterval& operator=(const TimeInterval&);

		constexpr static TimeInterval FromSeconds(double seconds);

		constexpr static TimeInterval Infinity();
		constexpr static TimeInterval NegativeInfinity();
		constexpr static TimeInterval Zero();
		constexpr static TimeInterval Invalid();
	private:
		int64 value;

		constexpr TimeInterval(int64 value);

		constexpr static TimeInterval CustomAdd(const TimeInterval& a, const TimeInterval& b);
		constexpr static TimeInterval CustomSub(const TimeInterval& a, const TimeInterval& b);

		friend class TimePoint;
	};
}