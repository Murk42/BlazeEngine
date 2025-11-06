#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"

namespace Blaze
{
	class ProgramTimePoint;

	class BLAZE_API TimeInterval
	{
	public:
		TimeInterval();
		TimeInterval(const TimeInterval& other);

		TimeInterval& SetSeconds(double seconds);
		double GetSeconds() const;
		TimeInterval& SetTicks(int64 ticks);
		int64 GetTicks() const;

		TimeInterval operator-() const;
		TimeInterval operator+(const TimeInterval&) const;
		TimeInterval operator-(const TimeInterval&) const;
		TimeInterval& operator+=(const TimeInterval&);
		TimeInterval& operator-=(const TimeInterval&);
		inline bool operator<(const TimeInterval& other) const { return value < other.value; }
		inline bool operator>(const TimeInterval& other) const { return value > other.value; }
		inline bool operator<=(const TimeInterval& other) const { return value <= other.value; }
		inline bool operator>=(const TimeInterval& other) const { return value >= other.value; }
		inline bool operator==(const TimeInterval& other) const { return value == other.value; }
		inline bool operator!=(const TimeInterval& other) const { return value != other.value; }

		TimeInterval& operator=(const TimeInterval&);

		static TimeInterval FromSeconds(double seconds);
		static TimeInterval FromTicks(int64 ticks);

		static TimeInterval Infinity();
		static TimeInterval NegativeInfinity();
		static TimeInterval Zero();
		static TimeInterval Invalid();
	private:
		int64 value;

		static TimeInterval CustomAdd(const TimeInterval& a, const TimeInterval& b);
		static TimeInterval CustomSub(const TimeInterval& a, const TimeInterval& b);
	};
}