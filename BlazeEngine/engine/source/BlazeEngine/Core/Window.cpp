#include "BlazeEngine/Core/Window.h"
#include "source/BlazeEngine/Internal/Engine.h"
#include "source/BlazeEngine/Internal/Conversions.h"

#include <SDL/SDL.h>

namespace Blaze
{				
	void* CreateWindow(StringView title, Vec2i size)
	{
		if (engine->App.initWindow != nullptr)
		{
			SDL_SetWindowTitle((SDL_Window*)engine->App.initWindow, title.Ptr());
			SDL_SetWindowSize((SDL_Window*)engine->App.initWindow, size.x, size.y);			
			void* copy = engine->App.initWindow;
			engine->App.initWindow = nullptr;
			return copy;
		}
		else
			return SDL_CreateWindow(title.Ptr(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, size.x, size.y, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
	}

	Window::Window()
	{
		engine->App.allWindows.push_back(this);				
		ptr = CreateWindow("Blaze Engine Application", { 640, 360 });
	}
	Window::Window(StringView title)
	{
		engine->App.allWindows.push_back(this);
		ptr = CreateWindow(title, { 640, 360 });
	}
	Window::Window(StringView title, Vec2i size)
	{
		engine->App.allWindows.push_back(this);
		ptr = CreateWindow(title, size);
	}

	Window::~Window()
	{
		auto i = std::find(engine->App.allWindows.begin(), engine->App.allWindows.end(), this);

		if (i != engine->App.allWindows.end())
			engine->App.allWindows.erase(i);

		SDL_DestroyWindow((SDL_Window*)ptr);
	}

	Vec2i Window::GetPos() const
	{
		Vec2i pos;
		SDL_GetWindowPosition((SDL_Window*)ptr, &pos.x, &pos.y);
		return pos;
	}
	Vec2i Window::GetSize() const
	{
		Vec2i size;
		SDL_GetWindowSize((SDL_Window*)ptr, &size.x, &size.y);
		return size;
	}
	String Window::GetTitle() const
	{
		return SDL_GetWindowTitle((SDL_Window*)ptr);
	}

	void Window::SetIcon(BitmapView bitmap)
	{
		if (bitmap.GetPixelType() != BitmapPixelType::Uint8 || bitmap.GetPixelFormat() != BitmapPixelFormat::RGBA)
		{
			BLAZE_INFO_LOG("Blaze Engine", "Unsuported bitmap type");
			return;
		}

		SDL_Surface* surface;
		surface = SDL_CreateRGBSurfaceWithFormat(0, bitmap.GetSize().x, bitmap.GetSize().y, 32, SDL_PIXELFORMAT_RGBA32);

		SDL_LockSurface(surface);
		size_t stride = bitmap.GetSize().x * GetFormatDepth(bitmap.GetPixelFormat());
		uint8* dst = (uint8*)surface->pixels;
		uint8* src = (uint8*)bitmap.GetPixels() + stride * (bitmap.GetSize().y - 1);		
		for (size_t y = 0; y < surface->h; ++y)
		{
			memcpy(dst, src, stride);
			dst += surface->pitch;
			src -= stride;
		}		
		SDL_UnlockSurface(surface);

		SDL_SetWindowIcon((SDL_Window*)ptr, surface);		
		SDL_FreeSurface(surface);
	}
	void Window::SetOpacity(float opacity)
	{		
		SDL_SetWindowOpacity((SDL_Window*)engine->App.initWindow, opacity);
	}
	void Window::SetPos(Vec2i s)
	{
		SDL_SetWindowPosition((SDL_Window*)ptr, s.x, s.y);
	}
	void Window::SetSize(Vec2i s)
	{
		SDL_SetWindowSize((SDL_Window*)ptr, s.x, s.y);
	}
	void Window::SetTitle(StringView title)
	{
		SDL_SetWindowTitle((SDL_Window*)ptr, title.Ptr());
	}
	void Window::SetMinimumSize(Vec2i size)
	{
		SDL_SetWindowMinimumSize((SDL_Window*)ptr, size.x, size.y);
	}
	void Window::SetMaximumSize(Vec2i size)
	{
		SDL_SetWindowMaximumSize((SDL_Window*)ptr, size.x, size.y);
	}

	void Window::Minimize()
	{
		SDL_MinimizeWindow((SDL_Window*)ptr);
	}
	void Window::Maximize()
	{
		SDL_MaximizeWindow((SDL_Window*)ptr);
	}

	void Window::Raise()
	{
		SDL_RaiseWindow((SDL_Window*)ptr);
	}

	void Window::ShowWindow(bool show)
	{
		if (show)
			SDL_ShowWindow((SDL_Window*)ptr);
		else
			SDL_HideWindow((SDL_Window*)ptr);
	}


	void Window::SetFullscreen(bool fullscreen)
	{
		if (fullscreen)
			SDL_SetWindowFullscreen((SDL_Window*)ptr, SDL_WINDOW_FULLSCREEN_DESKTOP);
		else
			SDL_SetWindowFullscreen((SDL_Window*)ptr, 0);
	}
	void Window::SetBorderless(bool lockMouseInside)
	{
		SDL_SetWindowFullscreen((SDL_Window*)ptr, 0);
		SDL_SetWindowBordered((SDL_Window*)ptr, SDL_FALSE);
		SDL_SetWindowGrab((SDL_Window*)ptr, (SDL_bool)lockMouseInside);
	}
	void Window::SetWindowed(bool resizable, bool lockMouseInside)
	{
		SDL_SetWindowFullscreen((SDL_Window*)ptr, 0);		
		SDL_SetWindowBordered((SDL_Window*)ptr, SDL_TRUE);
		SDL_SetWindowResizable((SDL_Window*)ptr, (SDL_bool)resizable);
		SDL_SetWindowGrab((SDL_Window*)ptr, (SDL_bool)lockMouseInside);
	}

	bool Window::IsFullscreen() { return SDL_GetWindowFlags((SDL_Window*)ptr) & SDL_WINDOW_FULLSCREEN_DESKTOP; }
	bool Window::IsBorderless() { return SDL_GetWindowFlags((SDL_Window*)ptr) & SDL_WINDOW_BORDERLESS; }
	bool Window::IsWindowed() { return !(SDL_GetWindowFlags((SDL_Window*)ptr) & SDL_WINDOW_BORDERLESS); }
	bool Window::IsResizable() { return SDL_GetWindowFlags((SDL_Window*)ptr) & SDL_WINDOW_RESIZABLE; }
	bool Window::IsMouseLockedInside() { return SDL_GetWindowGrab((SDL_Window*)ptr); }
	bool Window::IsMinmized() { return SDL_GetWindowFlags((SDL_Window*)ptr) & SDL_WINDOW_MINIMIZED; }
	bool Window::IsMaximized() { return SDL_GetWindowFlags((SDL_Window*)ptr) & SDL_WINDOW_MAXIMIZED; }
	bool Window::IsShown() { return SDL_GetWindowFlags((SDL_Window*)ptr) & SDL_WINDOW_SHOWN; }
}