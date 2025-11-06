#include "pch.h"
#include "BlazeEngine/Platform/Windows/Windows.h"
#include "BlazeEngine/Window/Window.h"
#include <Windows.h>
#include <SDL3/SDL.h>

namespace Blaze
{
	HDC GetMonitorHDC(HWND hwnd)
	{
		// Get the monitor handle (HMONITOR) where the window is located
		HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
		if (!hMonitor) {
			BLAZE_LOG_ERROR("Failed to get HMONITOR");
			return NULL;
		}

		// Get monitor information
		MONITORINFOEX monitorInfo;
		monitorInfo.cbSize = sizeof(MONITORINFOEX);
		if (!GetMonitorInfo(hMonitor, &monitorInfo)) {
			BLAZE_LOG_ERROR("Failed to get monitor info");
			return NULL;
		}

		// Get an HDC for the monitor
		HDC hdc = CreateDC(monitorInfo.szDevice, monitorInfo.szDevice, NULL, NULL);
		if (!hdc) {
			BLAZE_LOG_ERROR("Failed to get HDC for monitor");
			return NULL;
		}

		return hdc;
	}
	Vec2f GetMonitorDimensions(HDC hdc)
	{
		return Vec2f(GetDeviceCaps(hdc, HORZSIZE), GetDeviceCaps(hdc, VERTSIZE));
	}
	Vec2f GetWindowMonitorDimensions(const Window& window)
	{
		SDL_Window* Window = (SDL_Window*)window.GetHandle();
		HWND hwnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(Window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);

		HDC hdc = GetMonitorHDC(hwnd);

		Vec2f dimensions = GetMonitorDimensions(hdc);

		ReleaseDC(nullptr, hdc);

		return dimensions;
	}
}