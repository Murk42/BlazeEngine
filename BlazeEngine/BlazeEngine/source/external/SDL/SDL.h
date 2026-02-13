#pragma once
#include "BlazeEngineCore/String/StringView.h"
#include "BlazeEngineCore/Time/Timing.h"
#include "SDL.h"

namespace Blaze
{
	StringView GetSDLError();

	uint32 GetEngineSDLEventIdentifier();

	TimingResult InitializeSDL();
	void TerminateSDL();

	class SDLAllocator
	{
	public:
		static void* Allocate(uintMem size);
		static void Free(void*);
	};
}