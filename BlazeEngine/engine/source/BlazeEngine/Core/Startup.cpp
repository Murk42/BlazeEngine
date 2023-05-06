#include "BlazeEngine/Core/Startup.h"

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