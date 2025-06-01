#pragma once
#include <SDL3/SDL_video.h>
#include "BlazeEngine/Window/Display.h"

namespace Blaze
{
	Display::DisplayOrientation SDLToBlaze_DisplayOrientation(const SDL_DisplayOrientation& value);
	Display::DisplayPixelFormat SDLToBlaze_DisplayPixelFormat(const SDL_PixelFormat& value);
	Display::DisplayMode SDLToBlaze_DisplayMode(const SDL_DisplayMode& displayMode);
	SDL_PixelFormat BlazeToSDL_DisplayPixelFormat(const Display::DisplayPixelFormat& value);
	SDL_DisplayMode BlazeToSDL_DisplayMode(const Display::DisplayMode& displayMode);
	static StringView GetSDLEventName(uint32 type);
}