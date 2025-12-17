#pragma once
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_mouse.h>
#include "BlazeEngine/Runtime/IO/Display.h"
#include "BlazeEngine/Runtime/IO/Input.h"

namespace Blaze
{
	Display::DisplayOrientation SDL_GetBlazeDisplayOrientation(const SDL_DisplayOrientation& value);
	Display::DisplayPixelFormat SDL_GetBlazeDisplayPixelFormat(const SDL_PixelFormat& value);
	Display::DisplayMode SDL_GetBlazeDisplayMode(const SDL_DisplayMode& displayMode);
	SDL_PixelFormat SDL_GetSDLDisplayPixelFormat(const Display::DisplayPixelFormat& value);
	SDL_DisplayMode SDL_GetSDLDisplayMode(const Display::DisplayMode& displayMode);
	StringView SDL_GetEventName(uint32 type);
	Input::MouseButton SDL_GetBlazeMouseButton(int value);
	SDL_SystemCursor SDL_GetSDLSystemCursor(Input::CursorType value);
}