#include "pch.h"
#include "BlazeEngine/Window/Display.h"
#include "BlazeEngine/Internal/Libraries/SDL.h"
#include "BlazeEngine/Internal/Libraries/SDLConversions.h"

#include <SDL3/SDL.h>

namespace Blaze::Display
{		
	DisplayID GetPrimaryDisplay()
	{
		return SDL_GetPrimaryDisplay();		
	}
	StringViewUTF8 GetDisplayName(DisplayID id)
	{
		const char* ptr = SDL_GetDisplayName(id);

		if (ptr == nullptr)
			return StringViewUTF8();
		else
			return StringViewUTF8(ptr, strlen(ptr));
	}
	Rectf GetDisplayRect(DisplayID id)	
	{
		SDL_Rect rect;
		if (!SDL_GetDisplayBounds(id, &rect))
			BLAZE_ENGINE_ERROR("SDL_GetDisplayBounds failed. SDL returned error: \"" + GetSDLError() + "\"");

		return Rectf(rect.x, rect.y, rect.w, rect.h);
	}
	Array<DisplayID> GetDisplays()
	{
		int count;
		SDL_DisplayID* displaysPtr = SDL_GetDisplays(&count);

		Array<DisplayID> displays{ displaysPtr, (uintMem)count };

		SDL_free(displaysPtr);

		return displays;
	}
	Array<DisplayMode> GetFullscreenDisplayModes(DisplayID id)
	{
		int count;
		SDL_DisplayMode** displayModesPtr = SDL_GetFullscreenDisplayModes(id, &count);

		if (displayModesPtr == nullptr)
		{
			BLAZE_ENGINE_ERROR("SDL_GetFullscreenDisplayModes failed. SDL returned error: \"" + GetSDLError() + "\"");
			return Array<DisplayMode>();
		}

		Array<DisplayMode> displayModes{ (uintMem)count };		
		
		for (uintMem i = 0; i < count; ++i)
			if (!SDLToBlaze_DisplayMode(*displayModesPtr[i], displayModes[i]))
			{
				SDL_free(displayModesPtr);

				BLAZE_ENGINE_ERROR("Failed to convert SDL display mode to Blaze display mode");
				return Array<DisplayMode>();
			}

		SDL_free(displayModesPtr);

		return displayModes;
	}
	DisplayMode GetCurrentDisplayMode(DisplayID id)
	{
		auto ptr = SDL_GetCurrentDisplayMode(id);

		if (ptr == nullptr)
		{
			BLAZE_ENGINE_ERROR("SDL_GetCurrentDisplayMode failed. SDL returned error: \"" + GetSDLError() + "\"");
			return DisplayMode();
		}

		DisplayMode displayMode;
		if (!SDLToBlaze_DisplayMode(*ptr, displayMode))
		{
			BLAZE_ENGINE_ERROR("Failed to convert SDL display mode to Blaze display mode");
			return DisplayMode();
		}

		return displayMode;
	}
	DisplayMode GetDesktopDisplayMode(DisplayID id)
	{
		auto ptr = SDL_GetDesktopDisplayMode(id);

		if (ptr == nullptr)
		{
			BLAZE_ENGINE_ERROR("SDL_GetDesktopDisplayMode failed. SDL returned error: \"" + GetSDLError() + "\"");
			return DisplayMode();
		}

		DisplayMode displayMode;
		if (!SDLToBlaze_DisplayMode(*ptr, displayMode))
		{
			BLAZE_ENGINE_ERROR("Failed to convert SDL display mode to Blaze display mode");
			return DisplayMode();
		}

		return displayMode;
	}
	bool GetClosestFullscreenDisplayMode(DisplayID id, Vec2u size, float refreshRate, bool includeHighPixelDensityModes, DisplayMode& displayMode)
	{
		SDL_DisplayMode displayModeSDL;
		if (!SDL_GetClosestFullscreenDisplayMode(id, (int)size.x, (int)size.y, refreshRate, includeHighPixelDensityModes, &displayModeSDL))
		{
			BLAZE_ENGINE_ERROR("SDL_GetClosestFullscreenDisplayMode failed. SDL returned error: \"" + GetSDLError() + "\"");
			return false;
		}
		
		if (!SDLToBlaze_DisplayMode(displayModeSDL, displayMode))
		{
			BLAZE_ENGINE_ERROR("Failed to convert SDL display mode to Blaze display mode");
			return false;
		}
		
		return true;
	}
	DisplayOrientation GetNaturalDisplayOrientation(DisplayID id)
	{		
		DisplayOrientation out;

		if (!SDLToBlaze_DisplayOrientation(SDL_GetNaturalDisplayOrientation(id), out))
		{
			BLAZE_ENGINE_ERROR("Failed to convert SDL_DisplayOrientation enum to Blaze::DisplayOrientation");
			return DisplayOrientation::Unknown;
		}

		return out;
	}
	DisplayOrientation GetCurrentDisplayOrientation(DisplayID id)
	{
		DisplayOrientation out;

		if (!SDLToBlaze_DisplayOrientation(SDL_GetCurrentDisplayOrientation(id), out))
		{
			BLAZE_ENGINE_ERROR("Failed to convert SDL_DisplayOrientation enum to Blaze::DisplayOrientation");
			return DisplayOrientation::Unknown;
		}
		
		return out;
	}
	float GetDisplayContentScale(DisplayID id)
	{
		return SDL_GetDisplayContentScale(id);		
	}
}