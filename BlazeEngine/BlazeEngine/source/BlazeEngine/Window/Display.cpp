#include "pch.h"
#include "BlazeEngineCore/Debug/Logger.h"
#include "BlazeEngine/Window/Display.h"
#include "external/SDL/SDL.h"
#include "external/SDL/SDLConversions.h"

namespace Blaze::Display
{
	DisplayID GetPrimaryDisplay()
	{
		return SDL_GetPrimaryDisplay();
	}
	u8StringView GetDisplayName(DisplayID id)
	{
		const char* ptr = SDL_GetDisplayName(id);

		if (ptr == nullptr)
			return u8StringView();
		else
			return u8StringView(ptr, strlen(ptr));
	}
	Rectf GetDisplayRect(DisplayID id)
	{
		SDL_Rect rect;
		if (!SDL_GetDisplayBounds(id, &rect))
			BLAZE_LOG_ERROR("SDL_GetDisplayBounds failed. SDL returned error: \"" + GetSDLError() + "\"");

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
	DisplayData GetDisplayData(DisplayID id)
	{
		DisplayMode currentDisplayMode = SDLToBlaze_DisplayMode(*SDL_GetCurrentDisplayMode(id));

		return {
			.name = GetDisplayName(id),
			.desktopMode = SDLToBlaze_DisplayMode(*SDL_GetDesktopDisplayMode(id)),
			.currentMode = SDLToBlaze_DisplayMode(*SDL_GetCurrentDisplayMode(id)),
			.fullscreenDisplayModes = GetFullscreenDisplayModes(id),
			.rect = GetDisplayRect(id),
			.currentOrientation = SDLToBlaze_DisplayOrientation(SDL_GetCurrentDisplayOrientation(id)),
			.naturalOrientation = SDLToBlaze_DisplayOrientation(SDL_GetNaturalDisplayOrientation(id)),
			.contentScale = SDL_GetDisplayContentScale(id),
		};
	}
	Array<DisplayMode> GetFullscreenDisplayModes(DisplayID id)
	{
		int count;
		SDL_DisplayMode** displayModesPtr = SDL_GetFullscreenDisplayModes(id, &count);

		if (displayModesPtr == nullptr)
		{
			BLAZE_LOG_ERROR("SDL_GetFullscreenDisplayModes failed. SDL returned error: \"" + GetSDLError() + "\"");
			return Array<DisplayMode>();
		}

		Array<DisplayMode> displayModes((uintMem)count);

		for (uintMem i = 0; i < count; ++i)
			displayModes[i] = SDLToBlaze_DisplayMode(*displayModesPtr[i]);

		SDL_free(displayModesPtr);

		return displayModes;
	}
	DisplayMode GetCurrentDisplayMode(DisplayID id)
	{
		auto ptr = SDL_GetCurrentDisplayMode(id);

		if (ptr == nullptr)
		{
			BLAZE_LOG_ERROR("SDL_GetCurrentDisplayMode failed. SDL returned error: \"" + GetSDLError() + "\"");
			return DisplayMode();
		}

		return SDLToBlaze_DisplayMode(*ptr);
	}
	DisplayMode GetDesktopDisplayMode(DisplayID id)
	{
		auto ptr = SDL_GetDesktopDisplayMode(id);

		if (ptr == nullptr)
		{
			BLAZE_LOG_ERROR("SDL_GetDesktopDisplayMode failed. SDL returned error: \"" + GetSDLError() + "\"");
			return DisplayMode();
		}

		return SDLToBlaze_DisplayMode(*ptr);
	}
	DisplayMode GetClosestFullscreenDisplayMode(DisplayID id, Vec2u size, float refreshRate, bool includeHighPixelDensityModes)
	{
		SDL_DisplayMode displayModeSDL;
		if (!SDL_GetClosestFullscreenDisplayMode(id, (int)size.x, (int)size.y, refreshRate, includeHighPixelDensityModes, &displayModeSDL))
		{
			BLAZE_LOG_ERROR("SDL_GetClosestFullscreenDisplayMode failed. SDL returned error: \"" + GetSDLError() + "\"");
			return DisplayMode();
		}

		return SDLToBlaze_DisplayMode(displayModeSDL);
	}
	DisplayOrientation GetNaturalDisplayOrientation(DisplayID id)
	{
		return SDLToBlaze_DisplayOrientation(SDL_GetNaturalDisplayOrientation(id));
	}
	DisplayOrientation GetCurrentDisplayOrientation(DisplayID id)
	{
		return SDLToBlaze_DisplayOrientation(SDL_GetCurrentDisplayOrientation(id));
	}
	float GetDisplayContentScale(DisplayID id)
	{
		return SDL_GetDisplayContentScale(id);
	}
}