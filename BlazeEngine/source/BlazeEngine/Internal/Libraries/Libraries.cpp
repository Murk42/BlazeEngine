#include "source/BlazeEngine/Internal/Libraries/SDL.h"
#include "source/BlazeEngine/Internal/Libraries/GLEW.h"
#include "source/BlazeEngine/Internal/Libraries/FreeType.h"

#include "BlazeEngine/Utilities/Time.h"


namespace Blaze
{	
	TimingResult InitializeLibraries()
	{
		Timing timing{ "Engine libraries" };	
		
		timing.AddNode(InitializeSDL());
		//timing.AddNode(InitializeGLEW());
		//timing.AddNode(InitializeFreeType());
		
		return timing.GetTimingResult();
	}
	void TerminateLibraries()
	{
		//TerminateFreeType();
		//TerminateGLEW();
		TerminateSDL();
	}
}