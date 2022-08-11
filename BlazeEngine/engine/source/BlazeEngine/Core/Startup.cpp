#include "BlazeEngine/Core/Startup.h"

namespace Blaze
{
	namespace Startup
	{
		static StartupInfo startupInfo;

		StartupInfo GetStartupInfo()
		{			
			return startupInfo;
		}

	}
}

extern "C" BLAZE_API void SetStartupInfo(Blaze::Startup::StartupInfo info)
{
	Blaze::Startup::startupInfo = info;
}