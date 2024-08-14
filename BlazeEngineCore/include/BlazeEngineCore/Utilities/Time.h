#pragma once
#include <chrono>

namespace Blaze
{		
	class TimePoint;

	class BLAZE_CORE_API TimeInterval
	{
	public:
		TimeInterval();		
		TimeInterval(const TimeInterval&);

		TimeInterval& SetSeconds(double seconds);
		double ToSeconds() const;

		TimeInterval operator+(const TimeInterval&) const;
		TimeInterval operator-(const TimeInterval&) const;
		TimeInterval& operator+=(const TimeInterval&);
		TimeInterval& operator-=(const TimeInterval&);
		TimePoint operator+(const TimePoint&) const;
		inline bool operator<(const TimeInterval& other) const { return value < other.value; }
		inline bool operator>(const TimeInterval& other) const { return value > other.value; }
		inline bool operator<=(const TimeInterval& other) const { return value <= other.value; }
		inline bool operator>=(const TimeInterval& other) const { return value >= other.value; }
		inline bool operator==(const TimeInterval& other) const { return value == other.value; }
		inline bool operator!=(const TimeInterval& other) const { return value != other.value; }

		TimeInterval& operator=(const TimeInterval&);
	private:
		int64 value;

		friend class TimePoint;
	};
	
	class BLAZE_CORE_API TimePoint
	{			
	public:
		TimePoint();
		TimePoint(const TimePoint&);

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
		String FormatString(const char* format);		

		TimePoint operator+(const TimeInterval&) const;
		TimePoint operator-(const TimeInterval&) const;
		TimeInterval operator-(const TimePoint&) const;
		TimePoint& operator+=(const TimeInterval&);
		TimePoint& operator-=(const TimeInterval&);
		inline bool operator<(const TimePoint& other) const { return value < other.value; }
		inline bool operator>(const TimePoint& other) const { return value > other.value; }
		inline bool operator<=(const TimePoint& other) const { return value <= other.value; }
		inline bool operator>=(const TimePoint& other) const { return value >= other.value; }
		inline bool operator==(const TimePoint& other) const { return value == other.value; }
		inline bool operator!=(const TimePoint& other) const { return value != other.value; }

		TimePoint& operator=(const TimePoint&);

		static TimePoint GetCurrentWorldTime();		
	private:
		uint64 value;
	};
}