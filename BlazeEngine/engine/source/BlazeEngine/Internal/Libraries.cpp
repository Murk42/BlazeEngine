#include "source/BlazeEngine/Internal/Libraries.h"

#include "source/BlazeEngine/Internal/Libraries/SDL.h"
#include "source/BlazeEngine/Internal/Libraries/DevIL.h"
#include "source/BlazeEngine/Internal/Libraries/GLEW.h"
#include "source/BlazeEngine/Internal/Libraries/FreeType.h"

#include "BlazeEngine/Utilities/Time.h"


namespace Blaze
{	
	Startup::BlazeLibrariesInitInfo InitializeLibraries()
	{
		Startup::BlazeLibrariesInitInfo initInfo;
		TimePoint startTimePoint = TimePoint::GetWorldTime();

		initInfo.sdlInitInfo = InitializeSDL();
		initInfo.glewInitInfo = InitializeGLEW();
		initInfo.devILInitInfo = InitializeDevIL();
		initInfo.freeTypeInitInfo = InitializeFreeType();
		
		initInfo.initTime = TimePoint::GetWorldTime() - startTimePoint;
		return initInfo;
	}
	void TerminateLibraries()
	{
		TerminateFreeType();
		TerminateDevIL();
		TerminateGLEW();
		TerminateSDL();
	}
}