#include "pch.h"
#include "BlazeEngine/Internal/BlazeEngineContext.h"
#include "SDLConversions.h"

namespace Blaze
{
	Display::DisplayOrientation SDLToBlaze_DisplayOrientation(const SDL_DisplayOrientation& value)
	{
		switch (value)
		{
		case SDL_ORIENTATION_LANDSCAPE:          return Display::DisplayOrientation::Landscape;
		case SDL_ORIENTATION_LANDSCAPE_FLIPPED:  return Display::DisplayOrientation::LandscapeFlipped;
		case SDL_ORIENTATION_PORTRAIT:           return Display::DisplayOrientation::Portrait;
		case SDL_ORIENTATION_PORTRAIT_FLIPPED:   return Display::DisplayOrientation::PortraitFlipped;
		case SDL_ORIENTATION_UNKNOWN:            return Display::DisplayOrientation::Unknown;
		default:
			BLAZE_ENGINE_ERROR("Unrecognized SDL display orientation enum value");
			return Display::DisplayOrientation::Unknown;
		}

	}
	Display::DisplayPixelFormat SDLToBlaze_DisplayPixelFormat(const SDL_PixelFormat& value)
	{
		switch (value)
		{
		case SDL_PIXELFORMAT_UNKNOWN:       return Display::DisplayPixelFormat::Unknown;
		case SDL_PIXELFORMAT_INDEX1LSB:     return Display::DisplayPixelFormat::Index1LSB;
		case SDL_PIXELFORMAT_INDEX1MSB:     return Display::DisplayPixelFormat::Index1MSB;
		case SDL_PIXELFORMAT_INDEX2LSB:     return Display::DisplayPixelFormat::Index2LSB;
		case SDL_PIXELFORMAT_INDEX2MSB:     return Display::DisplayPixelFormat::Index2MSB;
		case SDL_PIXELFORMAT_INDEX4LSB:     return Display::DisplayPixelFormat::Index4LSB;
		case SDL_PIXELFORMAT_INDEX4MSB:     return Display::DisplayPixelFormat::Index4MSB;
		case SDL_PIXELFORMAT_INDEX8:        return Display::DisplayPixelFormat::Index8;
		case SDL_PIXELFORMAT_RGB332:        return Display::DisplayPixelFormat::RGB332;
		case SDL_PIXELFORMAT_XRGB4444:      return Display::DisplayPixelFormat::XRGB4444;
		case SDL_PIXELFORMAT_XBGR4444:      return Display::DisplayPixelFormat::XBGR4444;
		case SDL_PIXELFORMAT_XRGB1555:      return Display::DisplayPixelFormat::XRGB1555;
		case SDL_PIXELFORMAT_XBGR1555:      return Display::DisplayPixelFormat::XBGR1555;
		case SDL_PIXELFORMAT_ARGB4444:      return Display::DisplayPixelFormat::ARGB4444;
		case SDL_PIXELFORMAT_RGBA4444:      return Display::DisplayPixelFormat::RGBA4444;
		case SDL_PIXELFORMAT_ABGR4444:      return Display::DisplayPixelFormat::ABGR4444;
		case SDL_PIXELFORMAT_BGRA4444:      return Display::DisplayPixelFormat::BGRA4444;
		case SDL_PIXELFORMAT_ARGB1555:      return Display::DisplayPixelFormat::ARGB1555;
		case SDL_PIXELFORMAT_RGBA5551:      return Display::DisplayPixelFormat::RGBA5551;
		case SDL_PIXELFORMAT_ABGR1555:      return Display::DisplayPixelFormat::ABGR1555;
		case SDL_PIXELFORMAT_BGRA5551:      return Display::DisplayPixelFormat::BGRA5551;
		case SDL_PIXELFORMAT_RGB565:        return Display::DisplayPixelFormat::RGB565;
		case SDL_PIXELFORMAT_BGR565:        return Display::DisplayPixelFormat::BGR565;
		case SDL_PIXELFORMAT_RGB24:         return Display::DisplayPixelFormat::RGB24;
		case SDL_PIXELFORMAT_BGR24:         return Display::DisplayPixelFormat::BGR24;
		case SDL_PIXELFORMAT_XRGB8888:      return Display::DisplayPixelFormat::XRGB8888;
		case SDL_PIXELFORMAT_RGBX8888:      return Display::DisplayPixelFormat::RGBX8888;
		case SDL_PIXELFORMAT_XBGR8888:      return Display::DisplayPixelFormat::XBGR8888;
		case SDL_PIXELFORMAT_BGRX8888:      return Display::DisplayPixelFormat::BGRX8888;
		case SDL_PIXELFORMAT_ARGB8888:      return Display::DisplayPixelFormat::ARGB8888;
		case SDL_PIXELFORMAT_RGBA8888:      return Display::DisplayPixelFormat::RGBA8888;
		case SDL_PIXELFORMAT_ABGR8888:      return Display::DisplayPixelFormat::ABGR8888;
		case SDL_PIXELFORMAT_BGRA8888:      return Display::DisplayPixelFormat::BGRA8888;
		case SDL_PIXELFORMAT_XRGB2101010:   return Display::DisplayPixelFormat::XRGB2101010;
		case SDL_PIXELFORMAT_XBGR2101010:   return Display::DisplayPixelFormat::XBGR2101010;
		case SDL_PIXELFORMAT_ARGB2101010:   return Display::DisplayPixelFormat::ARGB2101010;
		case SDL_PIXELFORMAT_ABGR2101010:   return Display::DisplayPixelFormat::ABGR2101010;
		case SDL_PIXELFORMAT_RGB48:         return Display::DisplayPixelFormat::RGB48;
		case SDL_PIXELFORMAT_BGR48:         return Display::DisplayPixelFormat::BGR48;
		case SDL_PIXELFORMAT_RGBA64:        return Display::DisplayPixelFormat::RGBA64;
		case SDL_PIXELFORMAT_ARGB64:        return Display::DisplayPixelFormat::ARGB64;
		case SDL_PIXELFORMAT_BGRA64:        return Display::DisplayPixelFormat::BGRA64;
		case SDL_PIXELFORMAT_ABGR64:        return Display::DisplayPixelFormat::ABGR64;
		case SDL_PIXELFORMAT_RGB48_FLOAT:   return Display::DisplayPixelFormat::RGB48_float;
		case SDL_PIXELFORMAT_BGR48_FLOAT:   return Display::DisplayPixelFormat::BGR48_float;
		case SDL_PIXELFORMAT_RGBA64_FLOAT:  return Display::DisplayPixelFormat::RGBA64_float;
		case SDL_PIXELFORMAT_ARGB64_FLOAT:  return Display::DisplayPixelFormat::ARGB64_float;
		case SDL_PIXELFORMAT_BGRA64_FLOAT:  return Display::DisplayPixelFormat::BGRA64_float;
		case SDL_PIXELFORMAT_ABGR64_FLOAT:  return Display::DisplayPixelFormat::ABGR64_float;
		case SDL_PIXELFORMAT_RGB96_FLOAT:   return Display::DisplayPixelFormat::RGB96_float;
		case SDL_PIXELFORMAT_BGR96_FLOAT:   return Display::DisplayPixelFormat::BGR96_float;
		case SDL_PIXELFORMAT_RGBA128_FLOAT: return Display::DisplayPixelFormat::RGBA128_float;
		case SDL_PIXELFORMAT_ARGB128_FLOAT: return Display::DisplayPixelFormat::ARGB128_float;
		case SDL_PIXELFORMAT_BGRA128_FLOAT: return Display::DisplayPixelFormat::BGRA128_float;
		default:
			BLAZE_ENGINE_ERROR("Unrecognized SDL display pixel format enum value");
			return Display::DisplayPixelFormat::Unknown;
		}
	}
	Display::DisplayMode SDLToBlaze_DisplayMode(const SDL_DisplayMode& displayMode)
	{
		return {
			.displayID = displayMode.displayID,
			.pixelFormat = SDLToBlaze_DisplayPixelFormat(displayMode.format),
			.size = Vec2u(displayMode.w, displayMode.h),
			.pixelDensity = displayMode.pixel_density,
			.refreshRate = displayMode.refresh_rate,
			.refreshRateNumerator = (uint)displayMode.refresh_rate_numerator,
			.refreshRateDenominator = (uint)displayMode.refresh_rate_denominator,
		};
	}
	SDL_PixelFormat BlazeToSDL_DisplayPixelFormat(const Display::DisplayPixelFormat& value)
	{
		switch (value)
		{
		case Display::DisplayPixelFormat::Unknown:       return SDL_PIXELFORMAT_UNKNOWN;
		case Display::DisplayPixelFormat::Index1LSB:     return SDL_PIXELFORMAT_INDEX1LSB;
		case Display::DisplayPixelFormat::Index1MSB:     return SDL_PIXELFORMAT_INDEX1MSB;
		case Display::DisplayPixelFormat::Index2LSB:     return SDL_PIXELFORMAT_INDEX2LSB;
		case Display::DisplayPixelFormat::Index2MSB:     return SDL_PIXELFORMAT_INDEX2MSB;
		case Display::DisplayPixelFormat::Index4LSB:     return SDL_PIXELFORMAT_INDEX4LSB;
		case Display::DisplayPixelFormat::Index4MSB:     return SDL_PIXELFORMAT_INDEX4MSB;
		case Display::DisplayPixelFormat::Index8:        return SDL_PIXELFORMAT_INDEX8;
		case Display::DisplayPixelFormat::RGB332:        return SDL_PIXELFORMAT_RGB332;
		case Display::DisplayPixelFormat::XRGB4444:      return SDL_PIXELFORMAT_XRGB4444;
		case Display::DisplayPixelFormat::XBGR4444:      return SDL_PIXELFORMAT_XBGR4444;
		case Display::DisplayPixelFormat::XRGB1555:      return SDL_PIXELFORMAT_XRGB1555;
		case Display::DisplayPixelFormat::XBGR1555:      return SDL_PIXELFORMAT_XBGR1555;
		case Display::DisplayPixelFormat::ARGB4444:      return SDL_PIXELFORMAT_ARGB4444;
		case Display::DisplayPixelFormat::RGBA4444:      return SDL_PIXELFORMAT_RGBA4444;
		case Display::DisplayPixelFormat::ABGR4444:      return SDL_PIXELFORMAT_ABGR4444;
		case Display::DisplayPixelFormat::BGRA4444:      return SDL_PIXELFORMAT_BGRA4444;
		case Display::DisplayPixelFormat::ARGB1555:      return SDL_PIXELFORMAT_ARGB1555;
		case Display::DisplayPixelFormat::RGBA5551:      return SDL_PIXELFORMAT_RGBA5551;
		case Display::DisplayPixelFormat::ABGR1555:      return SDL_PIXELFORMAT_ABGR1555;
		case Display::DisplayPixelFormat::BGRA5551:      return SDL_PIXELFORMAT_BGRA5551;
		case Display::DisplayPixelFormat::RGB565:        return SDL_PIXELFORMAT_RGB565;
		case Display::DisplayPixelFormat::BGR565:        return SDL_PIXELFORMAT_BGR565;
		case Display::DisplayPixelFormat::RGB24:         return SDL_PIXELFORMAT_RGB24;
		case Display::DisplayPixelFormat::BGR24:         return SDL_PIXELFORMAT_BGR24;
		case Display::DisplayPixelFormat::XRGB8888:      return SDL_PIXELFORMAT_XRGB8888;
		case Display::DisplayPixelFormat::RGBX8888:      return SDL_PIXELFORMAT_RGBX8888;
		case Display::DisplayPixelFormat::XBGR8888:      return SDL_PIXELFORMAT_XBGR8888;
		case Display::DisplayPixelFormat::BGRX8888:      return SDL_PIXELFORMAT_BGRX8888;
		case Display::DisplayPixelFormat::ARGB8888:      return SDL_PIXELFORMAT_ARGB8888;
		case Display::DisplayPixelFormat::RGBA8888:      return SDL_PIXELFORMAT_RGBA8888;
		case Display::DisplayPixelFormat::ABGR8888:      return SDL_PIXELFORMAT_ABGR8888;
		case Display::DisplayPixelFormat::BGRA8888:      return SDL_PIXELFORMAT_BGRA8888;
		case Display::DisplayPixelFormat::XRGB2101010:   return SDL_PIXELFORMAT_XRGB2101010;
		case Display::DisplayPixelFormat::XBGR2101010:   return SDL_PIXELFORMAT_XBGR2101010;
		case Display::DisplayPixelFormat::ARGB2101010:   return SDL_PIXELFORMAT_ARGB2101010;
		case Display::DisplayPixelFormat::ABGR2101010:   return SDL_PIXELFORMAT_ABGR2101010;
		case Display::DisplayPixelFormat::RGB48:         return SDL_PIXELFORMAT_RGB48;
		case Display::DisplayPixelFormat::BGR48:         return SDL_PIXELFORMAT_BGR48;
		case Display::DisplayPixelFormat::RGBA64:        return SDL_PIXELFORMAT_RGBA64;
		case Display::DisplayPixelFormat::ARGB64:        return SDL_PIXELFORMAT_ARGB64;
		case Display::DisplayPixelFormat::BGRA64:        return SDL_PIXELFORMAT_BGRA64;
		case Display::DisplayPixelFormat::ABGR64:        return SDL_PIXELFORMAT_ABGR64;
		case Display::DisplayPixelFormat::RGB48_float:   return SDL_PIXELFORMAT_RGB48_FLOAT;
		case Display::DisplayPixelFormat::BGR48_float:   return SDL_PIXELFORMAT_BGR48_FLOAT;
		case Display::DisplayPixelFormat::RGBA64_float:  return SDL_PIXELFORMAT_RGBA64_FLOAT;
		case Display::DisplayPixelFormat::ARGB64_float:  return SDL_PIXELFORMAT_ARGB64_FLOAT;
		case Display::DisplayPixelFormat::BGRA64_float:  return SDL_PIXELFORMAT_BGRA64_FLOAT;
		case Display::DisplayPixelFormat::ABGR64_float:  return SDL_PIXELFORMAT_ABGR64_FLOAT;
		case Display::DisplayPixelFormat::RGB96_float:   return SDL_PIXELFORMAT_RGB96_FLOAT;
		case Display::DisplayPixelFormat::BGR96_float:   return SDL_PIXELFORMAT_BGR96_FLOAT;
		case Display::DisplayPixelFormat::RGBA128_float: return SDL_PIXELFORMAT_RGBA128_FLOAT;
		case Display::DisplayPixelFormat::ARGB128_float: return SDL_PIXELFORMAT_ARGB128_FLOAT;
		case Display::DisplayPixelFormat::BGRA128_float: return SDL_PIXELFORMAT_BGRA128_FLOAT;
		default:
			BLAZE_ENGINE_ERROR("Unrecognized Blaze display pixel format enum value");
			return SDL_PIXELFORMAT_UNKNOWN;
		}
	}
	SDL_DisplayMode BlazeToSDL_DisplayMode(const Display::DisplayMode& displayMode)
	{
		return {
			.displayID = displayMode.displayID,
			.format = BlazeToSDL_DisplayPixelFormat(displayMode.pixelFormat),
			.w = (int)displayMode.size.x,
			.h = (int)displayMode.size.y,
			.pixel_density = displayMode.pixelDensity,
			.refresh_rate = displayMode.refreshRate,
			.refresh_rate_numerator = (int)displayMode.refreshRateNumerator,
			.refresh_rate_denominator = (int)displayMode.refreshRateDenominator,
			.internal = nullptr
		};
	}
	StringView GetSDLEventName(uint32 type)
	{
		if (type == blazeEngineContext.mainThreadTaskEventIdentifier)
			return "MAIN_THREAD_TASK_EVENT";
		if (type == blazeEngineContext.clientThreadExitEventIdentifier)
			return "CLIENT_THREAD_EXIT_EVENT";

		switch (type)
		{
		case SDL_EVENT_QUIT: return "QUIT_EVENT";
		case SDL_EVENT_TERMINATING: return "TERMINATING_EVENT";
		case SDL_EVENT_LOW_MEMORY: return "LOW_MEMORY_EVENT";
		case SDL_EVENT_WILL_ENTER_BACKGROUND: return "WILL_ENTER_BACKGROUND_EVENT";
		case SDL_EVENT_DID_ENTER_BACKGROUND: return "DID_ENTER_BACKGROUND_EVENT";
		case SDL_EVENT_WILL_ENTER_FOREGROUND: return "WILL_ENTER_FOREGROUND_EVENT";
		case SDL_EVENT_DID_ENTER_FOREGROUND: return "DID_ENTER_FOREGROUND_EVENT";
		case SDL_EVENT_LOCALE_CHANGED: return "LOCALE_CHANGED_EVENT";
		case SDL_EVENT_SYSTEM_THEME_CHANGED: return "SYSTEM_THEME_CHANGED_EVENT";
		case SDL_EVENT_DISPLAY_ORIENTATION: return "DISPLAY_ORIENTATION_EVENT";
		case SDL_EVENT_DISPLAY_ADDED: return "DISPLAY_ADDED_EVENT";
		case SDL_EVENT_DISPLAY_REMOVED: return "DISPLAY_REMOVED_EVENT";
		case SDL_EVENT_DISPLAY_MOVED: return "DISPLAY_MOVED_EVENT";
		case SDL_EVENT_DISPLAY_DESKTOP_MODE_CHANGED: return "DISPLAY_DESKTOP_MODE_CHANGED_EVENT";
		case SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED: return "DISPLAY_CURRENT_MODE_CHANGED_EVENT";
		case SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED: return "DISPLAY_CONTENT_SCALE_CHANGED_EVENT";
		case SDL_EVENT_WINDOW_SHOWN: return "WINDOW_SHOWN_EVENT";
		case SDL_EVENT_WINDOW_HIDDEN: return "WINDOW_HIDDEN_EVENT";
		case SDL_EVENT_WINDOW_EXPOSED: return "WINDOW_EXPOSED_EVENT";
		case SDL_EVENT_WINDOW_MOVED: return "WINDOW_MOVED_EVENT";
		case SDL_EVENT_WINDOW_RESIZED: return "WINDOW_RESIZED_EVENT";
		case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: return "WINDOW_PIXEL_SIZE_CHANGED_EVENT";
		case SDL_EVENT_WINDOW_METAL_VIEW_RESIZED: return "WINDOW_METAL_VIEW_RESIZED_EVENT";
		case SDL_EVENT_WINDOW_MINIMIZED: return "WINDOW_MINIMIZED_EVENT";
		case SDL_EVENT_WINDOW_MAXIMIZED: return "WINDOW_MAXIMIZED_EVENT";
		case SDL_EVENT_WINDOW_RESTORED: return "WINDOW_RESTORED_EVENT";
		case SDL_EVENT_WINDOW_MOUSE_ENTER: return "WINDOW_MOUSE_ENTER_EVENT";
		case SDL_EVENT_WINDOW_MOUSE_LEAVE: return "WINDOW_MOUSE_LEAVE_EVENT";
		case SDL_EVENT_WINDOW_FOCUS_GAINED: return "WINDOW_FOCUS_GAINED_EVENT";
		case SDL_EVENT_WINDOW_FOCUS_LOST: return "WINDOW_FOCUS_LOST_EVENT";
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED: return "WINDOW_CLOSE_REQUESTED_EVENT";
		case SDL_EVENT_WINDOW_HIT_TEST: return "WINDOW_HIT_TEST_EVENT";
		case SDL_EVENT_WINDOW_ICCPROF_CHANGED: return "WINDOW_ICCPROF_CHANGED_EVENT";
		case SDL_EVENT_WINDOW_DISPLAY_CHANGED: return "WINDOW_DISPLAY_CHANGED_EVENT";
		case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED: return "WINDOW_DISPLAY_SCALE_CHANGED_EVENT";
		case SDL_EVENT_WINDOW_SAFE_AREA_CHANGED: return "WINDOW_SAFE_AREA_CHANGED_EVENT";
		case SDL_EVENT_WINDOW_OCCLUDED: return "WINDOW_OCCLUDED_EVENT";
		case SDL_EVENT_WINDOW_ENTER_FULLSCREEN: return "WINDOW_ENTER_FULLSCREEN_EVENT";
		case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN: return "WINDOW_LEAVE_FULLSCREEN_EVENT";
		case SDL_EVENT_WINDOW_DESTROYED: return "WINDOW_DESTROYED_EVENT";
		case SDL_EVENT_WINDOW_HDR_STATE_CHANGED: return "WINDOW_HDR_STATE_CHANGED_EVENT";
		case SDL_EVENT_KEY_DOWN: return "KEY_DOWN_EVENT";
		case SDL_EVENT_KEY_UP: return "KEY_UP_EVENT";
		case SDL_EVENT_TEXT_EDITING: return "TEXT_EDITING_EVENT";
		case SDL_EVENT_TEXT_INPUT: return "TEXT_INPUT_EVENT";
		case SDL_EVENT_KEYMAP_CHANGED: return "KEYMAP_CHANGED_EVENT";
		case SDL_EVENT_KEYBOARD_ADDED: return "KEYBOARD_ADDED_EVENT";
		case SDL_EVENT_KEYBOARD_REMOVED: return "KEYBOARD_REMOVED_EVENT";
		case SDL_EVENT_TEXT_EDITING_CANDIDATES: return "TEXT_EDITING_CANDIDATES_EVENT";
		case SDL_EVENT_MOUSE_MOTION: return "MOUSE_MOTION_EVENT";
		case SDL_EVENT_MOUSE_BUTTON_DOWN: return "MOUSE_BUTTON_DOWN_EVENT";
		case SDL_EVENT_MOUSE_BUTTON_UP: return "MOUSE_BUTTON_UP_EVENT";
		case SDL_EVENT_MOUSE_WHEEL: return "MOUSE_WHEEL_EVENT";
		case SDL_EVENT_MOUSE_ADDED: return "MOUSE_ADDED_EVENT";
		case SDL_EVENT_MOUSE_REMOVED: return "MOUSE_REMOVED_EVENT";
		case SDL_EVENT_JOYSTICK_AXIS_MOTION: return "JOYSTICK_AXIS_MOTION_EVENT";
		case SDL_EVENT_JOYSTICK_BALL_MOTION: return "JOYSTICK_BALL_MOTION_EVENT";
		case SDL_EVENT_JOYSTICK_HAT_MOTION: return "JOYSTICK_HAT_MOTION_EVENT";
		case SDL_EVENT_JOYSTICK_BUTTON_DOWN: return "JOYSTICK_BUTTON_DOWN_EVENT";
		case SDL_EVENT_JOYSTICK_BUTTON_UP: return "JOYSTICK_BUTTON_UP_EVENT";
		case SDL_EVENT_JOYSTICK_ADDED: return "JOYSTICK_ADDED_EVENT";
		case SDL_EVENT_JOYSTICK_REMOVED: return "JOYSTICK_REMOVED_EVENT";
		case SDL_EVENT_JOYSTICK_BATTERY_UPDATED: return "JOYSTICK_BATTERY_UPDATED_EVENT";
		case SDL_EVENT_JOYSTICK_UPDATE_COMPLETE: return "JOYSTICK_UPDATE_COMPLETE_EVENT";
		case SDL_EVENT_GAMEPAD_AXIS_MOTION: return "GAMEPAD_AXIS_MOTION_EVENT";
		case SDL_EVENT_GAMEPAD_BUTTON_DOWN: return "GAMEPAD_BUTTON_DOWN_EVENT";
		case SDL_EVENT_GAMEPAD_BUTTON_UP: return "GAMEPAD_BUTTON_UP_EVENT";
		case SDL_EVENT_GAMEPAD_ADDED: return "GAMEPAD_ADDED_EVENT";
		case SDL_EVENT_GAMEPAD_REMOVED: return "GAMEPAD_REMOVED_EVENT";
		case SDL_EVENT_GAMEPAD_REMAPPED: return "GAMEPAD_REMAPPED_EVENT";
		case SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN: return "GAMEPAD_TOUCHPAD_DOWN_EVENT";
		case SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION: return "GAMEPAD_TOUCHPAD_MOTION_EVENT";
		case SDL_EVENT_GAMEPAD_TOUCHPAD_UP: return "GAMEPAD_TOUCHPAD_UP_EVENT";
		case SDL_EVENT_GAMEPAD_SENSOR_UPDATE: return "GAMEPAD_SENSOR_UPDATE_EVENT";
		case SDL_EVENT_GAMEPAD_UPDATE_COMPLETE: return "GAMEPAD_UPDATE_COMPLETE_EVENT";
		case SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED: return "GAMEPAD_STEAM_HANDLE_UPDATED_EVENT";
		case SDL_EVENT_FINGER_DOWN: return "FINGER_DOWN_EVENT";
		case SDL_EVENT_FINGER_UP: return "FINGER_UP_EVENT";
		case SDL_EVENT_FINGER_MOTION: return "FINGER_MOTION_EVENT";
		case SDL_EVENT_FINGER_CANCELED: return "FINGER_CANCELED_EVENT";
		case SDL_EVENT_CLIPBOARD_UPDATE: return "CLIPBOARD_UPDATE_EVENT";
		case SDL_EVENT_DROP_FILE: return "DROP_FILE_EVENT";
		case SDL_EVENT_DROP_TEXT: return "DROP_TEXT_EVENT";
		case SDL_EVENT_DROP_BEGIN: return "DROP_BEGIN_EVENT";
		case SDL_EVENT_DROP_COMPLETE: return "DROP_COMPLETE_EVENT";
		case SDL_EVENT_DROP_POSITION: return "DROP_POSITION_EVENT";
		case SDL_EVENT_AUDIO_DEVICE_ADDED: return "AUDIO_DEVICE_ADDED_EVENT";
		case SDL_EVENT_AUDIO_DEVICE_REMOVED: return "AUDIO_DEVICE_REMOVED_EVENT";
		case SDL_EVENT_AUDIO_DEVICE_FORMAT_CHANGED: return "AUDIO_DEVICE_FORMAT_CHANGED_EVENT";
		case SDL_EVENT_SENSOR_UPDATE: return "SENSOR_UPDATE_EVENT";
		case SDL_EVENT_PEN_PROXIMITY_IN: return "PEN_PROXIMITY_IN_EVENT";
		case SDL_EVENT_PEN_PROXIMITY_OUT: return "PEN_PROXIMITY_OUT_EVENT";
		case SDL_EVENT_PEN_DOWN: return "PEN_DOWN_EVENT";
		case SDL_EVENT_PEN_UP: return "PEN_UP_EVENT";
		case SDL_EVENT_PEN_BUTTON_DOWN: return "PEN_BUTTON_DOWN_EVENT";
		case SDL_EVENT_PEN_BUTTON_UP: return "PEN_BUTTON_UP_EVENT";
		case SDL_EVENT_PEN_MOTION: return "PEN_MOTION_EVENT";
		case SDL_EVENT_PEN_AXIS: return "PEN_AXIS_EVENT";
		case SDL_EVENT_CAMERA_DEVICE_ADDED: return "CAMERA_DEVICE_ADDED_EVENT";
		case SDL_EVENT_CAMERA_DEVICE_REMOVED: return "CAMERA_DEVICE_REMOVED_EVENT";
		case SDL_EVENT_CAMERA_DEVICE_APPROVED: return "CAMERA_DEVICE_APPROVED_EVENT";
		case SDL_EVENT_CAMERA_DEVICE_DENIED: return "CAMERA_DEVICE_DENIED_EVENT";
		case SDL_EVENT_RENDER_TARGETS_RESET: return "RENDER_TARGETS_RESET_EVENT";
		case SDL_EVENT_RENDER_DEVICE_RESET: return "RENDER_DEVICE_RESET_EVENT";
		case SDL_EVENT_RENDER_DEVICE_LOST: return "RENDER_DEVICE_LOST_EVENT";
		default: return "UNRECOGNIZED EVENT";
		}
	}
}