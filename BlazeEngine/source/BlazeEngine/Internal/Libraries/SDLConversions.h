#pragma once
#include <SDL3/SDL.h>
#include "BlazeEngine/Window/Display.h"

namespace Blaze
{
	static constexpr bool SDLToBlaze_DisplayOrientation(const SDL_DisplayOrientation& value, Display::DisplayOrientation& out)
	{
		switch (value)
		{
		case SDL_ORIENTATION_LANDSCAPE:          out = Display::DisplayOrientation::Landscape; return true;
		case SDL_ORIENTATION_LANDSCAPE_FLIPPED:  out = Display::DisplayOrientation::LandscapeFlipped; return true;
		case SDL_ORIENTATION_PORTRAIT:           out = Display::DisplayOrientation::Portrait; return true;
		case SDL_ORIENTATION_PORTRAIT_FLIPPED:   out = Display::DisplayOrientation::PortraitFlipped; return true;
		case SDL_ORIENTATION_UNKNOWN:            out = Display::DisplayOrientation::Unknown; return true;
		default: return false;
		}

	}
	static constexpr bool SDLToBlaze_DisplayPixelFormat(const SDL_PixelFormat& value, Display::DisplayPixelFormat& out)
	{
		switch (value)
		{
		case SDL_PIXELFORMAT_UNKNOWN:       out = Display::DisplayPixelFormat::Unknown; return true;
		case SDL_PIXELFORMAT_INDEX1LSB:     out = Display::DisplayPixelFormat::Index1LSB; return true;
		case SDL_PIXELFORMAT_INDEX1MSB:     out = Display::DisplayPixelFormat::Index1MSB; return true;
		case SDL_PIXELFORMAT_INDEX2LSB:     out = Display::DisplayPixelFormat::Index2LSB; return true;
		case SDL_PIXELFORMAT_INDEX2MSB:     out = Display::DisplayPixelFormat::Index2MSB; return true;
		case SDL_PIXELFORMAT_INDEX4LSB:     out = Display::DisplayPixelFormat::Index4LSB; return true;
		case SDL_PIXELFORMAT_INDEX4MSB:     out = Display::DisplayPixelFormat::Index4MSB; return true;
		case SDL_PIXELFORMAT_INDEX8:        out = Display::DisplayPixelFormat::Index8; return true;
		case SDL_PIXELFORMAT_RGB332:        out = Display::DisplayPixelFormat::RGB332; return true;
		case SDL_PIXELFORMAT_XRGB4444:      out = Display::DisplayPixelFormat::XRGB4444; return true;
		case SDL_PIXELFORMAT_XBGR4444:      out = Display::DisplayPixelFormat::XBGR4444; return true;
		case SDL_PIXELFORMAT_XRGB1555:      out = Display::DisplayPixelFormat::XRGB1555; return true;
		case SDL_PIXELFORMAT_XBGR1555:      out = Display::DisplayPixelFormat::XBGR1555; return true;
		case SDL_PIXELFORMAT_ARGB4444:      out = Display::DisplayPixelFormat::ARGB4444; return true;
		case SDL_PIXELFORMAT_RGBA4444:      out = Display::DisplayPixelFormat::RGBA4444; return true;
		case SDL_PIXELFORMAT_ABGR4444:      out = Display::DisplayPixelFormat::ABGR4444; return true;
		case SDL_PIXELFORMAT_BGRA4444:      out = Display::DisplayPixelFormat::BGRA4444; return true;
		case SDL_PIXELFORMAT_ARGB1555:      out = Display::DisplayPixelFormat::ARGB1555; return true;
		case SDL_PIXELFORMAT_RGBA5551:      out = Display::DisplayPixelFormat::RGBA5551; return true;
		case SDL_PIXELFORMAT_ABGR1555:      out = Display::DisplayPixelFormat::ABGR1555; return true;
		case SDL_PIXELFORMAT_BGRA5551:      out = Display::DisplayPixelFormat::BGRA5551; return true;
		case SDL_PIXELFORMAT_RGB565:        out = Display::DisplayPixelFormat::RGB565; return true;
		case SDL_PIXELFORMAT_BGR565:        out = Display::DisplayPixelFormat::BGR565; return true;
		case SDL_PIXELFORMAT_RGB24:         out = Display::DisplayPixelFormat::RGB24; return true;
		case SDL_PIXELFORMAT_BGR24:         out = Display::DisplayPixelFormat::BGR24; return true;
		case SDL_PIXELFORMAT_XRGB8888:      out = Display::DisplayPixelFormat::XRGB8888; return true;
		case SDL_PIXELFORMAT_RGBX8888:      out = Display::DisplayPixelFormat::RGBX8888; return true;
		case SDL_PIXELFORMAT_XBGR8888:      out = Display::DisplayPixelFormat::XBGR8888; return true;
		case SDL_PIXELFORMAT_BGRX8888:      out = Display::DisplayPixelFormat::BGRX8888; return true;
		case SDL_PIXELFORMAT_ARGB8888:      out = Display::DisplayPixelFormat::ARGB8888; return true;
		case SDL_PIXELFORMAT_RGBA8888:      out = Display::DisplayPixelFormat::RGBA8888; return true;
		case SDL_PIXELFORMAT_ABGR8888:      out = Display::DisplayPixelFormat::ABGR8888; return true;
		case SDL_PIXELFORMAT_BGRA8888:      out = Display::DisplayPixelFormat::BGRA8888; return true;
		case SDL_PIXELFORMAT_XRGB2101010:   out = Display::DisplayPixelFormat::XRGB2101010; return true;
		case SDL_PIXELFORMAT_XBGR2101010:   out = Display::DisplayPixelFormat::XBGR2101010; return true;
		case SDL_PIXELFORMAT_ARGB2101010:   out = Display::DisplayPixelFormat::ARGB2101010; return true;
		case SDL_PIXELFORMAT_ABGR2101010:   out = Display::DisplayPixelFormat::ABGR2101010; return true;
		case SDL_PIXELFORMAT_RGB48:         out = Display::DisplayPixelFormat::RGB48; return true;
		case SDL_PIXELFORMAT_BGR48:         out = Display::DisplayPixelFormat::BGR48; return true;
		case SDL_PIXELFORMAT_RGBA64:        out = Display::DisplayPixelFormat::RGBA64; return true;
		case SDL_PIXELFORMAT_ARGB64:        out = Display::DisplayPixelFormat::ARGB64; return true;
		case SDL_PIXELFORMAT_BGRA64:        out = Display::DisplayPixelFormat::BGRA64; return true;
		case SDL_PIXELFORMAT_ABGR64:        out = Display::DisplayPixelFormat::ABGR64; return true;
		case SDL_PIXELFORMAT_RGB48_FLOAT:   out = Display::DisplayPixelFormat::RGB48_float; return true;
		case SDL_PIXELFORMAT_BGR48_FLOAT:   out = Display::DisplayPixelFormat::BGR48_float; return true;
		case SDL_PIXELFORMAT_RGBA64_FLOAT:  out = Display::DisplayPixelFormat::RGBA64_float; return true;
		case SDL_PIXELFORMAT_ARGB64_FLOAT:  out = Display::DisplayPixelFormat::ARGB64_float; return true;
		case SDL_PIXELFORMAT_BGRA64_FLOAT:  out = Display::DisplayPixelFormat::BGRA64_float; return true;
		case SDL_PIXELFORMAT_ABGR64_FLOAT:  out = Display::DisplayPixelFormat::ABGR64_float; return true;
		case SDL_PIXELFORMAT_RGB96_FLOAT:   out = Display::DisplayPixelFormat::RGB96_float; return true;
		case SDL_PIXELFORMAT_BGR96_FLOAT:   out = Display::DisplayPixelFormat::BGR96_float; return true;
		case SDL_PIXELFORMAT_RGBA128_FLOAT: out = Display::DisplayPixelFormat::RGBA128_float; return true;
		case SDL_PIXELFORMAT_ARGB128_FLOAT: out = Display::DisplayPixelFormat::ARGB128_float; return true;
		case SDL_PIXELFORMAT_BGRA128_FLOAT: out = Display::DisplayPixelFormat::BGRA128_float; return true;
		default: return false;
		}
	}
	static constexpr bool SDLToBlaze_DisplayMode(const SDL_DisplayMode& displayMode, Display::DisplayMode& out)
	{
		Display::DisplayPixelFormat pixelFormat;

		if (!SDLToBlaze_DisplayPixelFormat(displayMode.format, pixelFormat))
			return false;

		out = {
			.displayID = displayMode.displayID,
			.pixelFormat = pixelFormat,
			.size = Vec2u(displayMode.w, displayMode.h),
			.pixelDensity = displayMode.pixel_density,
			.refreshRate = displayMode.refresh_rate,
			.refreshRateNumerator = (uint)displayMode.refresh_rate_numerator,
			.refreshRateDenominator = (uint)displayMode.refresh_rate_denominator,
		};

		return true;
	}
	static StringView GetSDLEventName(uint32 type);
}