#include "BlazeEngine/Debug/Breakpoint.h"

#ifdef BLAZE_PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace Blaze::Debug
{
	void Breakpoint()
	{
#ifdef BLAZE_PLATFORM_WINDOWS
		DebugBreak();
#else
#error
#endif
	}
}