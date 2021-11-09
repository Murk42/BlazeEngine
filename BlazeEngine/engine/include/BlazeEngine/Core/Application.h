#pragma once
#include "BlazeEngine/Core/EngineCore.h"

int BLAZE_API main();

namespace Blaze
{				
	class Log;
	class Window;

	namespace Application
	{		
		BLAZE_API void SetTargetFramerate(unsigned fps);
		BLAZE_API void SetTargetDeltaTime(double deltaTime);

		BLAZE_API void Stop();

		BLAZE_API unsigned GetTargetDeltaTime();
		BLAZE_API unsigned GetFPS();
		BLAZE_API double GetDeltaTime();
	}
}
