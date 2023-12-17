#pragma once
#include "BlazeEngine/Window/WindowSDL.h"

namespace Blaze
{
#ifdef BLAZE_SDL_WINDOW_DEFAULT
	using Window = WindowSDL;
#endif
}