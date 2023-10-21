#pragma once
#include "BlazeEngine/Core/WindowSDL.h"

namespace Blaze
{
#ifdef BLAZE_SDL_WINDOW_DEFAULT
	using Window = WindowSDL;
#endif
}