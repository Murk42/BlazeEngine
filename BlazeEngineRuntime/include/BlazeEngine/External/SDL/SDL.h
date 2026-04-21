#pragma once
#include "BlazeEngine/Core/String/StringView.h"
#include "BlazeEngine/Core/Time/TimingTree.h"
#include "SDL3/SDL.h"

namespace Blaze
{
	StringView SDL_GetError();

	bool InitializeSDL(TimingTree& timingTree);
	void TerminateSDL();

	class SDLAllocator
	{
	public:
		static void* Allocate(uintMem size);
		static void Free(void*);
	};
}