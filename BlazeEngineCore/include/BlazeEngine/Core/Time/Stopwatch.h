#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"

namespace Blaze
{
	class BLAZE_API Stopwatch
	{
	public:
		Stopwatch();
		Stopwatch(const Stopwatch&);

		double Reset();
		double GetTime();

		Stopwatch& operator= (const Stopwatch&);
	private:
		uint64 value;
	};

}