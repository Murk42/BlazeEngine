#pragma once
#include "BlazeEngineCore/DataStructures/StringView.h"
#include "BlazeEngineCore/Utilities/Timing.h"

namespace Blaze
{	
	StringView GetSDLError();

	void PostSDLMainThreadTaskEvent();
	void PostSDLClientThreadExitEvent();

	TimingResult InitializeSDL();
	void TerminateSDL();

	class SDLAllocator
	{
	public:
		static void* Allocate(uintMem size);
		static void Free(void*);
	};
}