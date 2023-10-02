#include "pch.h"
#include "BlazeEngine/Core/Window.h"
#include "BlazeEngine/Internal/Conversions.h"
#include "BlazeEngine/Internal/WindowCreation.h"

#include <SDL2/SDL.h>

namespace Blaze
{	
	WindowHandle RequestWindowCreation(WindowCreationData data)
	{
		return windowCreationQueue.Request(data);
	}
	void RequestWindowDestruction(WindowHandle window)
	{
		windowDestructionQueue.Request({ window });
	}

	WindowHandle CreateWindowHandle(StringView title, Vec2i pos, Vec2i size, WindowHandleGraphicsaAPI graphicsAPI)
	{
		return RequestWindowCreation({ title, pos, size, graphicsAPI });
	}
	void DestroyWindowHandle(WindowHandle handle)
	{
		return RequestWindowDestruction(handle);
	}
	
	Window::Window(WindowHandle handle)
		: windowHandle(handle)
	{
		SDL_SetWindowData((SDL_Window*)windowHandle, "Blaze", this);
	}		
	Window::~Window()
	{		
		SDL_SetWindowData((SDL_Window*)windowHandle, "Blaze", nullptr);		
	}

	Vec2i Window::GetPos() const
	{
		Vec2i pos;
		SDL_GetWindowPosition((SDL_Window*)windowHandle, &pos.x, &pos.y);
		SDL_Rect rect;
		SDL_GetDisplayBounds(0, &rect);
		pos.y = rect.h - pos.y - GetSize().y;
		return pos;
	}
	Vec2i Window::GetSize() const
	{
		Vec2i size;
		SDL_GetWindowSize((SDL_Window*)windowHandle, &size.x, &size.y);
		return size;
	}
	String Window::GetTitle() const
	{
		return SDL_GetWindowTitle((SDL_Window*)windowHandle);
	}

	Result Window::SetIcon(BitmapView bitmap)
	{

		if (bitmap.GetPixelType() != BitmapColorComponentType::Uint8 || bitmap.GetPixelFormat() != BitmapColorFormat::RGBA)
			return BLAZE_WARNING_LOG("Blaze Engine", "Unsuported bitmap type");

		SDL_Surface* surface;
		surface = SDL_CreateRGBSurfaceWithFormat(0, bitmap.GetSize().x, bitmap.GetSize().y, 32, SDL_PIXELFORMAT_RGBA32);


		SDL_LockSurface(surface);
		size_t stride = bitmap.GetSize().x * BitmapColorFormatComponentCount(bitmap.GetPixelFormat());

		uint8* dst = (uint8*)surface->pixels;
		uint8* src = (uint8*)bitmap.GetPixels() + stride * (bitmap.GetSize().y - 1);
		for (size_t y = 0; y < surface->h; ++y)
		{
			memcpy(dst, src, stride);
			dst += surface->pitch;
			src -= stride;
		}
		SDL_UnlockSurface(surface);

		SDL_SetWindowIcon((SDL_Window*)windowHandle, surface);
		SDL_FreeSurface(surface);

		return Result();
	}
	void Window::SetOpacity(float opacity)
	{
		SDL_SetWindowOpacity((SDL_Window*)windowHandle, opacity);
	}
	void Window::SetPos(Vec2i s)
	{
		s.y = GetSize().y - s.y;
		SDL_SetWindowPosition((SDL_Window*)windowHandle, s.x, s.y);
	}
	void Window::SetSize(Vec2i s)
	{
		SDL_SetWindowSize((SDL_Window*)windowHandle, s.x, s.y);
	}
	void Window::SetTitle(StringView title)
	{
		SDL_SetWindowTitle((SDL_Window*)windowHandle, title.Ptr());
	}
	void Window::SetMinimumSize(Vec2i size)
	{
		SDL_SetWindowMinimumSize((SDL_Window*)windowHandle, size.x, size.y);
	}
	void Window::SetMaximumSize(Vec2i size)
	{
		SDL_SetWindowMaximumSize((SDL_Window*)windowHandle, size.x, size.y);
	}

	void Window::Minimize()
	{
		SDL_MinimizeWindow((SDL_Window*)windowHandle);
	}
	void Window::Maximize()
	{
		SDL_MaximizeWindow((SDL_Window*)windowHandle);
	}

	void Window::Raise()
	{
		SDL_RaiseWindow((SDL_Window*)windowHandle);
	}

	void Window::ShowWindow(bool show)
	{
		if (show)
			SDL_ShowWindow((SDL_Window*)windowHandle);
		else
			SDL_HideWindow((SDL_Window*)windowHandle);
	}

	void Window::SetWindowFullscreenMode(bool fullscreen)
	{
		if (fullscreen)
			SDL_SetWindowFullscreen((SDL_Window*)windowHandle, SDL_WINDOW_FULLSCREEN);
		else
			SDL_SetWindowFullscreen((SDL_Window*)windowHandle, 0);		
	}

	void Window::SetWindowBorderFlag(bool hasBorder)
	{
		SDL_SetWindowBordered((SDL_Window*)windowHandle, SDL_FALSE);
	}

	void Window::SetWindowResizableFlag(bool resizable)
	{
		SDL_SetWindowResizable((SDL_Window*)windowHandle, (SDL_bool)resizable);
	}

	void Window::SetLockMouseFlag(bool lockMouse)
	{
		SDL_SetWindowGrab((SDL_Window*)windowHandle, (SDL_bool)lockMouse);
	}	

	bool Window::IsFullscreen() { return SDL_GetWindowFlags((SDL_Window*)windowHandle) & SDL_WINDOW_FULLSCREEN_DESKTOP; }
	bool Window::IsBorderless() { return SDL_GetWindowFlags((SDL_Window*)windowHandle) & SDL_WINDOW_BORDERLESS; }	
	bool Window::IsResizable() { return SDL_GetWindowFlags((SDL_Window*)windowHandle) & SDL_WINDOW_RESIZABLE; }
	bool Window::IsMouseLocked() { return SDL_GetWindowGrab((SDL_Window*)windowHandle); }
	bool Window::IsMinmized() { return SDL_GetWindowFlags((SDL_Window*)windowHandle) & SDL_WINDOW_MINIMIZED; }
	bool Window::IsMaximized() { return SDL_GetWindowFlags((SDL_Window*)windowHandle) & SDL_WINDOW_MAXIMIZED; }
	bool Window::IsShown() { return SDL_GetWindowFlags((SDL_Window*)windowHandle) & SDL_WINDOW_SHOWN; }

	uint Window::GetWindowVideoDisplayIndex()
	{
		return SDL_GetWindowDisplayIndex((SDL_Window*)windowHandle);
	}

	DisplayMode Window::GetWindowDisplayMode()
	{
		SDL_DisplayMode mode;
		SDL_GetWindowDisplayMode((SDL_Window*)windowHandle, &mode);
		DisplayMode out;

		Result result;
		out.format = BlazeDisplayPixelFormat(mode.format, result);
		out.refreshRate = mode.refresh_rate;
		out.size = Vec2i(mode.w, mode.h);

		return out;
	}
	void Window::SetWindowDisplayMode(DisplayMode mode)
	{
		Result result;

		SDL_DisplayMode out;
		out.format = SDLDisplayPixelFormat(mode.format, result);
		out.w = mode.size.x;
		out.h = mode.size.y;
		out.refresh_rate = mode.refreshRate;
		out.driverdata = nullptr;
		SDL_SetWindowDisplayMode((SDL_Window*)windowHandle, &out);
	}			
}