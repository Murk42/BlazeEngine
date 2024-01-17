#include "pch.h"
#include "BlazeEngine/Internal/Libraries/SDL.h"
#include "BlazeEngine/Internal/Libraries/FreeType.h"
#include "BlazeEngine/Internal/Libraries/Sail.h"

namespace Blaze
{	
	TimingResult InitializeLibraries()
	{
		Timing timing{ "Engine libraries" };	
		
		timing.AddNode(InitializeSDL());		
		timing.AddNode(InitializeFreeType());
		timing.AddNode(InitializeSail());
		
		return timing.GetTimingResult();
	}
	void TerminateLibraries()
	{
		TerminateSail();
		TerminateFreeType();
		TerminateSDL();
	}
}