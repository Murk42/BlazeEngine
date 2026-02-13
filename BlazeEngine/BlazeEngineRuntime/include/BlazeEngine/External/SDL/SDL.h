#pragma once
#include "BlazeEngine/Core/String/StringView.h"
#include "BlazeEngine/Core/Time/Timing.h"
#include "SDL3/SDL.h"

namespace Blaze
{
	StringView SDL_GetError();

	TimingResult InitializeSDL();
	void TerminateSDL();

	class SDLAllocator
	{
	public:
		static void* Allocate(uintMem size);
		static void Free(void*);
	};
}