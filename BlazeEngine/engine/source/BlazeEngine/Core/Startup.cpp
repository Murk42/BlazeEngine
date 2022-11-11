#include "BlazeEngine/Core/Startup.h"
#include "source/BlazeEngine/Internal/EngineData.h"

namespace Blaze
{
	namespace Startup
	{		
		BlazeInitInfo GetInitInfo()
		{
			return engineData->initInfo;
		}

	}
}