#include "pch.h"
#include "BlazeEngine/Core/Time/Stopwatch.h"

namespace Blaze
{
	Stopwatch::Stopwatch()
		: value(0)
	{
		static_assert(sizeof(LARGE_INTEGER) == sizeof(value));
		QueryPerformanceCounter((LARGE_INTEGER*)&value);
	}
	Stopwatch::Stopwatch(const Stopwatch& t)
		: value(t.value)
	{
	}
	double Stopwatch::Reset()
	{
		LARGE_INTEGER now;
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&now);

		double duration = double(now.QuadPart - value) / frequency.QuadPart;
		value = now.QuadPart;

		return duration;
	}
	double Stopwatch::GetTime()
	{
		LARGE_INTEGER now;
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&now);
		double duration = double(now.QuadPart - value) / frequency.QuadPart;
		return duration;
	}
	Stopwatch& Stopwatch::operator= (const Stopwatch& t)
	{
		value = t.value;
		return *this;
	}
}