#pragma once
#include "BlazeEngine/Core/EngineCore.h"

namespace Blaze
{
	namespace Startup
	{
		struct BlazeLibrariesInitInfo
		{
			struct SDLInitInfo
			{
				double initTime;
			} sdlInitInfo;
			struct GLEWInitInfo
			{
				double windowInitTime;
				double contextInitTime;

				double initTime;
			} glewInitInfo;
			struct DevILInitInfo
			{
				double initTime;
			} devILInitInfo;
			struct FreeTypeInitInfo
			{
				double initTime;
			} freeTypeInitInfo;
			double initTime;
		};
		struct ConsoleInitInfo
		{
			double initTime;
		};
		struct InputInitInfo
		{
			double initTime;
		};		
		struct GraphicsInitInfo
		{
			double initTime;
		};
		struct BlazeInitInfo
		{
			BlazeLibrariesInitInfo libraryInitInfo;
			ConsoleInitInfo consoleInitInfo;
			InputInitInfo inputInitInfo;
			GraphicsInitInfo graphicsInitInfo;
			double initTime;
		};

		BLAZE_API BlazeInitInfo GetInitInfo();
	}
}