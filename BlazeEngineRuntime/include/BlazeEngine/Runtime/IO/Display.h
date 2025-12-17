#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/Common/Rect.h"
#include "BlazeEngine/Core/Container/Array.h"
#include "BlazeEngine/Core/String/String.h"
#include "BlazeEngine/Core/Math/Vector.h"

namespace Blaze::Display
{
	using DisplayID = uint32;

	enum class DisplayPixelFormat
	{
		Unknown = 0,
		Index1LSB = 0x11100100u,
		Index1MSB = 0x11200100u,
		Index2LSB = 0x1c100200u,
		Index2MSB = 0x1c200200u,
		Index4LSB = 0x12100400u,
		Index4MSB = 0x12200400u,
		Index8 = 0x13000801u,
		RGB332 = 0x14110801u,
		XRGB4444 = 0x15120c02u,
		XBGR4444 = 0x15520c02u,
		XRGB1555 = 0x15130f02u,
		XBGR1555 = 0x15530f02u,
		ARGB4444 = 0x15321002u,
		RGBA4444 = 0x15421002u,
		ABGR4444 = 0x15721002u,
		BGRA4444 = 0x15821002u,
		ARGB1555 = 0x15331002u,
		RGBA5551 = 0x15441002u,
		ABGR1555 = 0x15731002u,
		BGRA5551 = 0x15841002u,
		RGB565 = 0x15151002u,
		BGR565 = 0x15551002u,
		RGB24 = 0x17101803u,
		BGR24 = 0x17401803u,
		XRGB8888 = 0x16161804u,
		RGBX8888 = 0x16261804u,
		XBGR8888 = 0x16561804u,
		BGRX8888 = 0x16661804u,
		ARGB8888 = 0x16362004u,
		RGBA8888 = 0x16462004u,
		ABGR8888 = 0x16762004u,
		BGRA8888 = 0x16862004u,
		XRGB2101010 = 0x16172004u,
		XBGR2101010 = 0x16572004u,
		ARGB2101010 = 0x16372004u,
		ABGR2101010 = 0x16772004u,
		RGB48 = 0x18103006u,
		BGR48 = 0x18403006u,
		RGBA64 = 0x18204008u,
		ARGB64 = 0x18304008u,
		BGRA64 = 0x18504008u,
		ABGR64 = 0x18604008u,
		RGB48_float = 0x1a103006u,
		BGR48_float = 0x1a403006u,
		RGBA64_float = 0x1a204008u,
		ARGB64_float = 0x1a304008u,
		BGRA64_float = 0x1a504008u,
		ABGR64_float = 0x1a604008u,
		RGB96_float = 0x1b10600cu,
		BGR96_float = 0x1b40600cu,
		RGBA128_float = 0x1b208010u,
		ARGB128_float = 0x1b308010u,
		BGRA128_float = 0x1b508010u,
		ABGR128_float = 0x1b608010u
	};

	struct DisplayMode
	{
		DisplayID displayID;                 /* The display this mode is associated with */
		DisplayPixelFormat pixelFormat;
		Vec2u size;
		float pixelDensity;                  /* Scale converting size to pixels(e.g.a 1920x1080 mode with 2.0 scale would have 3840x2160 pixels) */
		float refreshRate;
		uint refreshRateNumerator;           /* Precise refresh rate numerator (or 0 for unspecified)  */
		uint refreshRateDenominator;         /* Precise refresh rate denominator */
	};

	enum class DisplayOrientation
	{
		Unknown = 0,
		Landscape = 1,          /* The display is in landscape mode, with the right side up, relative to portrait mode */
		LandscapeFlipped = 2,   /* The display is in landscape mode, with the left side up, relative to portrait mode */
		Portrait = 3,           /* The display is in portrait mode */
		PortraitFlipped = 4     /* The display is in portrait mode, upside down */
	};

	struct DisplayData
	{
		u8String name;
		/*
			There's a difference between this and <currentMode> when a window is fullscreen and has changed the resolution. In that case this
			value is the previous native display mode, and not the current display mode.
		*/
		DisplayMode desktopMode;
		/*
			There's a difference between this and <desktopMode> when a window is fullscreen and has changed the resolution. In that case this
			value is the current display mode, and not the previous native display mode.
		*/
		DisplayMode currentMode;
		Array<DisplayMode> fullscreenDisplayModes;
		Rectf rect;
		DisplayOrientation currentOrientation;
		DisplayOrientation naturalOrientation;
		float contentScale;
	};

	struct DisplayEvent
	{
		enum class Type
		{
			DisplayAdded,
			DisplayRemoved,
			OrientationChanged,
			Moved,
			DesktopModeChanged,
			CurrentModeChanged,
			ContentScaleChanged
		};
		DisplayID displayID;
		uint64 timeNS;
		Type type;
	};

	Array<DisplayID> GetDisplays();
	DisplayData GetDisplayData(DisplayID id);

	DisplayID GetPrimaryDisplay();
	u8StringView GetDisplayName(DisplayID id);
	Rectf GetDisplayRect(DisplayID id);
	Array<DisplayID> GetDisplays();
	Array<DisplayMode> GetFullscreenDisplayModes(DisplayID id);

	/*
		There's a difference between this function and GetDesktopDisplayMode() when a window is fullscreen and has changed the resolution. In that case this
		function will return the current display mode, and not the previous native display mode.
	*/
	DisplayMode GetCurrentDisplayMode(DisplayID id);

	/*
		There's a difference between this function and GetCurrentDisplayMode() when a window is fullscreen and has changed the resolution. In that case this
		function will return the previous native display mode, and not the current display mode.
	*/
	DisplayMode GetDesktopDisplayMode(DisplayID id);

	/*
		The available display modes are scanned and `closest` is filled in with the
		closest mode matching the requested mode and returned. The mode format and
		refresh rate default to the desktop mode if they are set to 0. The modes
		are scanned with size being first priority, format being second priority,
		and finally checking the refresh rate. If all the available modes are too
		small, then false is returned.
	*/
	DisplayMode GetClosestFullscreenDisplayMode(DisplayID id, Vec2u size, float refreshRate, bool includeHighPixelDensityModes);

	DisplayOrientation GetNaturalDisplayOrientation(DisplayID id);
	DisplayOrientation GetCurrentDisplayOrientation(DisplayID id);

	float GetDisplayContentScale(DisplayID id);
}