#include "source/BlazeEngine/Internal/Libraries.h"

#include "source/BlazeEngine/Internal/Libraries/SDL.h"
#include "source/BlazeEngine/Internal/Libraries/DevIL.h"
#include "source/BlazeEngine/Internal/Libraries/GLEW.h"
#include "source/BlazeEngine/Internal/Libraries/FreeType.h"

namespace Blaze
{
	void InitializeLibraries()
	{
		InitializeSDL();
		InitializeGLEW();
		InitializeDevIL();
		InitializeFreeType();
	}
	void TerminateLibraries()
	{
		TerminateFreeType();
		TerminateDevIL();
		TerminateGLEW();
		TerminateSDL();
	}
}