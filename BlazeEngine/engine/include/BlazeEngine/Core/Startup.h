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
		struct RendererInitInfo
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
			RendererInitInfo rendererInitInfo;
			GraphicsInitInfo graphicsInitInfo;
			double initTime;
		};
		struct StartupInfo
		{
			double blazeLoadTime;
			double clientLibrariesLoadTime;
			double clientLoadTime;

			double initializationTime; //Sum of all load times			

			BlazeInitInfo blazeInitInfo;

		};		

		BLAZE_API StartupInfo GetStartupInfo();
	}
}