#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/String/String.h"

namespace Blaze
{
	class TimeInterval;

	class BLAZE_API ProgramTimePoint
	{
	public:
		ProgramTimePoint();
		ProgramTimePoint(const ProgramTimePoint&) = default;
		ProgramTimePoint(ProgramTimePoint&&) noexcept = default;

		uint64 GetTicks() const;
		ProgramTimePoint& SetTicks(uint64 tick);

		/*
		%a	Abbreviated weekday name *																Thu
		%A	Full weekday name *																		Thursday
		%b	Abbreviated month name *																Aug
		%B	Full month name *																		August
		%c	Date and time representation *															Thu Aug 23 14:55:02 2001
		%C	Year divided by 100 and truncated to integer (00-99)									20
		%d	Day of the month, zero-padded (01-31)													23
		%D	Short MM/DD/YY date, equivalent to %m/%d/%y												08/23/01
		%e	Day of the month, space-padded ( 1-31)													23
		%F	Short YYYY-MM-DD date, equivalent to %Y-%m-%d											2001-08-23
		%g	Week-based year, last two digits (00-99)												01
		%G	Week-based year																			2001
		%h	Abbreviated month name * (same as %b)													Aug
		%H	Hour in 24h format (00-23)																14
		%I	Hour in 12h format (01-12)																02
		%j	Day of the year (001-366)																235
		%m	Month as a decimal number (01-12)														08
		%M	Minute (00-59)																			55
		%p	AM or PM designation																	PM
		%r	12-hour clock time *																	02:55:02 pm
		%R	24-hour HH:MM time, equivalent to %H:%M													14:55
		%S	Second (00-61)																			02
		%T	ISO 8601 time format (HH:MM:SS), equivalent to %H:%M:%S									14:55:02
		%u	ISO 8601 weekday as number with Monday as 1 (1-7)										4
		%U	Week number with the first Sunday as the first day of week one (00-53)					33
		%V	ISO 8601 week number (01-53)															34
		%w	Weekday as a decimal number with Sunday as 0 (0-6)										4
		%W	Week number with the first Monday as the first day of week one (00-53)					34
		%x	Date representation *																	08/23/01
		%X	TimePoint representation *																	14:55:02
		%y	Year, last two digits (00-99)															01
		%Y	Year																					2001
		%z	ISO 8601 offset from UTC in timezone (1 minute=1, 1 hour=100)							+100
		%Z	Timezone name or abbreviation *															CDT
		%%	A % sign																				%
		*/
		String Format(StringView specifiers) const;

		TimeInterval operator-(const ProgramTimePoint&) const;
		ProgramTimePoint operator+(const TimeInterval&) const;
		ProgramTimePoint operator-(const TimeInterval&) const;
		ProgramTimePoint& operator+=(const TimeInterval&);
		ProgramTimePoint& operator-=(const TimeInterval&);
		inline bool operator<(const ProgramTimePoint& other) const { return value < other.value; }
		inline bool operator>(const ProgramTimePoint& other) const { return value > other.value; }
		inline bool operator<=(const ProgramTimePoint& other) const { return value <= other.value; }
		inline bool operator>=(const ProgramTimePoint& other) const { return value >= other.value; }
		inline bool operator==(const ProgramTimePoint& other) const { return value == other.value; }
		inline bool operator!=(const ProgramTimePoint& other) const { return value != other.value; }

		ProgramTimePoint& operator=(const ProgramTimePoint&) = default;
		ProgramTimePoint& operator=(ProgramTimePoint&&) noexcept = default;

		static ProgramTimePoint FromTicks(uint64 ticks);
	private:
		uint64 value;

		static ProgramTimePoint CustomAdd(const ProgramTimePoint& a, const TimeInterval& b);
		static ProgramTimePoint CustomSub(const ProgramTimePoint& a, const TimeInterval& b);
		static TimeInterval CustomSub(const ProgramTimePoint& a, const ProgramTimePoint& b);

		friend class TimeInterval;
	};
}