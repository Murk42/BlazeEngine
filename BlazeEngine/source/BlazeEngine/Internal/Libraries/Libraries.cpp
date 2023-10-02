#include "pch.h"
#include "BlazeEngine/Internal/Libraries/SDL.h"
#include "BlazeEngine/Internal/Libraries/GLEW.h"
#include "BlazeEngine/Internal/Libraries/FreeType.h"

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