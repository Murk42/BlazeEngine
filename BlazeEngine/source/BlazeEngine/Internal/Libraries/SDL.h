#pragma once

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