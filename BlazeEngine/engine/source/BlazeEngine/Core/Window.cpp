#include "BlazeEngine/Core/Window.h"
#include "source/BlazeEngine/Internal/Conversions.h"

#include <SDL/SDL.h>

namespace Blaze
{				
	void* GetOpenGLInitWindow();	
	static bool firstWindowCreated = false;

	const std::vector<Window*>& GetAllWindows()
	{
		return engineData->allWindows;
	}

	bool WasFirstWindowCreated()
	{
		return firstWindowCreated;
	}

	void* CreateWindow(StringView title, Vec2i size)
	{
		if (!firstWindowCreated)
		{
			void* ptr = GetOpenGLInitWindow();
			SDL_SetWindowTitle((SDL_Window*)ptr, title.Ptr());
			SDL_SetWindowSize((SDL_Window*)ptr, size.x, size.y);									
			firstWindowCreated = true;
			return ptr;
		}
		else
			return SDL_CreateWindow(title.Ptr(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, size.x, size.y, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
	}

	Window::Window()
	{
		engineData->allWindows.emplace_back(this);
		ptr = CreateWindow("Blaze Engine Application", { 640, 360 });
	}
	Window::Window(StringView title)
	{
		engineData->allWindows.emplace_back(this);
		ptr = CreateWindow(title, { 640, 360 });
	}
	Window::Window(StringView title, Vec2i size)
	{
		engineData->allWindows.emplace_back(this);
		ptr = CreateWindow(title, size);
	}

	Window::~Window()
	{
		auto i = std::find(engineData->allWindows.begin(), engineData->allWindows.end(), this);

		if (i != engineData->allWindows.end())
			engineData->allWindows.erase(i);

		SDL_DestroyWindow((SDL_Window*)ptr);
	}

	Vec2i Window::GetPos() const
	{
		Vec2i pos;
		SDL_GetWindowPosition((SDL_Window*)ptr, &pos.x, & pos.y);
		SDL_Rect rect;
		SDL_GetDisplayBounds(0, &rect);		
		pos.y = rect.h - pos.y - GetSize().y;
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

	Result Window::SetIcon(BitmapView bitmap)
	{
		Result result;

		if (bitmap.GetPixelType() != BitmapPixelType::Uint8 || bitmap.GetPixelFormat() != BitmapPixelFormat::RGBA)		
			return BLAZE_WARNING_LOG("Blaze Engine", "Unsuported bitmap type");					

		SDL_Surface* surface;
		surface = SDL_CreateRGBSurfaceWithFormat(0, bitmap.GetSize().x, bitmap.GetSize().y, 32, SDL_PIXELFORMAT_RGBA32);


		SDL_LockSurface(surface);
		size_t stride = bitmap.GetSize().x * GetFormatDepth(bitmap.GetPixelFormat(), result);
		CHECK_RESULT(result);
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

		return Result();
	}
	void Window::SetOpacity(float opacity)
	{		
		SDL_SetWindowOpacity((SDL_Window*)ptr, opacity);
	}
	void Window::SetPos(Vec2i s)
	{
		s.y = GetSize().y - s.y;
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
			SDL_SetWindowFullscreen((SDL_Window*)ptr, SDL_WINDOW_FULLSCREEN);
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

	uint Window::GetWindowVideoDisplayIndex()
	{
		return SDL_GetWindowDisplayIndex((SDL_Window*)ptr);
	}

	DisplayMode Window::GetWindowDisplayMode()
	{
		SDL_DisplayMode mode;
		SDL_GetWindowDisplayMode((SDL_Window*)ptr, &mode);
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
		SDL_SetWindowDisplayMode((SDL_Window*)ptr, &out);
	}
}