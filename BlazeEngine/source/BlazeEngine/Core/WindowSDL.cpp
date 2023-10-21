#include "pch.h"
#include "BlazeEngine/Core/WindowSDL.h"
#include "BlazeEngine/Internal/Conversions.h"
#include "BlazeEngine/Internal/WindowCreation.h"

#include <SDL2/SDL.h> 

namespace Blaze
{	
	WindowSDL::WindowSDLHandle RequestSDLWindowCreation(WindowSDLCreationData data)
	{
		return windowSDLCreationQueue.Request(data);
	}
	void RequestSDLWindowDestruction(WindowSDL::WindowSDLHandle window)
	{
		windowSDLDestructionQueue.Request({ window });
	}

	WindowSDL::WindowSDLHandle CreateWindowSDLHandle(StringView title, Vec2i pos, Vec2i size, WindowHandleGraphicsaAPI graphicsAPI)
	{
		return RequestSDLWindowCreation({ title, pos, size, graphicsAPI });
	}
	void DestroyWindowSDLHandle(WindowSDL::WindowSDLHandle handle)
	{
		return RequestSDLWindowDestruction(handle);
	}
		
	WindowSDL::WindowSDL()
		: handle(nullptr)
	{

	}
	WindowSDL::WindowSDL(StringView title, Vec2i pos, Vec2i size, WindowHandleGraphicsaAPI graphicsAPI)
	{
		handle = CreateWindowSDLHandle(title, pos, size, graphicsAPI);		

		//If any events were reported before this line they wont be reported to the client.		
		SDL_SetWindowData((SDL_Window*)handle, "Blaze", this);

	}
	WindowSDL::WindowSDL(WindowSDL&& other) noexcept
	{
		handle = other.handle;
		SDL_SetWindowData((SDL_Window*)handle, "Blaze", this);

		other.handle = nullptr;
	}
	WindowSDL::~WindowSDL()
	{
		Destroy();
	}

	void WindowSDL::Destroy()
	{
		if (handle == nullptr)
			return;
		SDL_SetWindowData((SDL_Window*)handle, "Blaze", nullptr);
		DestroyWindowSDLHandle(handle);
		handle = nullptr;
	}

	Vec2i WindowSDL::GetPos() const
	{
		if (handle == nullptr)
			return Vec2i();

		Vec2i pos;
		SDL_GetWindowPosition((SDL_Window*)handle, &pos.x, &pos.y);
		SDL_Rect rect;
		SDL_GetDisplayBounds(0, &rect);
		pos.y = rect.h - pos.y - GetSize().y;
		return pos;
	}
	Vec2i WindowSDL::GetSize() const
	{
		if (handle == nullptr)
			return Vec2i();

		Vec2i size;
		SDL_GetWindowSize((SDL_Window*)handle, &size.x, &size.y);
		return size;
	}
	String WindowSDL::GetTitle() const
	{
		if (handle == nullptr)
			return "";
		return SDL_GetWindowTitle((SDL_Window*)handle);
	}

	Result WindowSDL::SetIcon(BitmapView bitmap)
	{
		if (handle == nullptr)
			return Result();

		if (bitmap.GetPixelType() != BitmapColorComponentType::Uint8 || bitmap.GetPixelFormat() != BitmapColorFormat::RGBA)
			return BLAZE_WARNING_LOG("Blaze Engine", "Unsupported bitmap type");

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

		SDL_SetWindowIcon((SDL_Window*)handle, surface);
		SDL_FreeSurface(surface);

		return Result();
	}
	void WindowSDL::SetOpacity(float opacity)
	{
		if (handle == nullptr)
			return;

		SDL_SetWindowOpacity((SDL_Window*)handle, opacity);
	}
	void WindowSDL::SetPos(Vec2i s)
	{
		if (handle == nullptr)
			return;

		s.y = GetSize().y - s.y;
		SDL_SetWindowPosition((SDL_Window*)handle, s.x, s.y);
	}
	void WindowSDL::SetSize(Vec2i s)
	{
		if (handle == nullptr)
			return;

		SDL_SetWindowSize((SDL_Window*)handle, s.x, s.y);
	}
	void WindowSDL::SetTitle(StringView title)
	{
		if (handle == nullptr)
			return;

		SDL_SetWindowTitle((SDL_Window*)handle, title.Ptr());
	}
	void WindowSDL::SetMinimumSize(Vec2i size)
	{
		if (handle == nullptr)
			return;

		SDL_SetWindowMinimumSize((SDL_Window*)handle, size.x, size.y);
	}
	void WindowSDL::SetMaximumSize(Vec2i size)
	{
		if (handle == nullptr)
			return;

		SDL_SetWindowMaximumSize((SDL_Window*)handle, size.x, size.y);
	}

	void WindowSDL::Minimize()
	{
		if (handle == nullptr)
			return;

		SDL_MinimizeWindow((SDL_Window*)handle);
	}
	void WindowSDL::Maximize()
	{
		if (handle == nullptr)
			return;

		SDL_MaximizeWindow((SDL_Window*)handle);
	}

	void WindowSDL::Raise()
	{
		if (handle == nullptr)
			return;

		SDL_RaiseWindow((SDL_Window*)handle);
	}

	void WindowSDL::ShowWindow(bool show)
	{
		if (handle == nullptr)
			return;

		if (show)
			SDL_ShowWindow((SDL_Window*)handle);
		else
			SDL_HideWindow((SDL_Window*)handle);
	}

	void WindowSDL::SetWindowFullscreenMode(bool fullscreen)
	{
		if (handle == nullptr)
			return;

		if (fullscreen)
			SDL_SetWindowFullscreen((SDL_Window*)handle, SDL_WINDOW_FULLSCREEN);
		else
			SDL_SetWindowFullscreen((SDL_Window*)handle, 0);		
	}

	void WindowSDL::SetWindowBorderFlag(bool hasBorder)
	{
		if (handle == nullptr)
			return;

		SDL_SetWindowBordered((SDL_Window*)handle, SDL_FALSE);
	}

	void WindowSDL::SetWindowResizableFlag(bool resizable)
	{
		if (handle == nullptr)
			return;

		SDL_SetWindowResizable((SDL_Window*)handle, (SDL_bool)resizable);
	}

	void WindowSDL::SetLockMouseFlag(bool lockMouse)
	{
		if (handle == nullptr)
			return;

		SDL_SetWindowGrab((SDL_Window*)handle, (SDL_bool)lockMouse);
	}	

	bool WindowSDL::IsFullscreen() { if (handle == nullptr) return false; return SDL_GetWindowFlags((SDL_Window*)handle) & SDL_WINDOW_FULLSCREEN_DESKTOP; }
	bool WindowSDL::IsBorderless() { if (handle == nullptr) return false; return SDL_GetWindowFlags((SDL_Window*)handle) & SDL_WINDOW_BORDERLESS; }
	bool WindowSDL::IsResizable() { if (handle == nullptr) return false; return SDL_GetWindowFlags((SDL_Window*)handle) & SDL_WINDOW_RESIZABLE; }
	bool WindowSDL::IsMouseLocked() { if (handle == nullptr) return false; return SDL_GetWindowGrab((SDL_Window*)handle); }
	bool WindowSDL::IsMinmized() { if (handle == nullptr) return false; return SDL_GetWindowFlags((SDL_Window*)handle) & SDL_WINDOW_MINIMIZED; }
	bool WindowSDL::IsMaximized() { if (handle == nullptr) return false; return SDL_GetWindowFlags((SDL_Window*)handle) & SDL_WINDOW_MAXIMIZED; }
	bool WindowSDL::IsShown() { if (handle == nullptr) return false; return SDL_GetWindowFlags((SDL_Window*)handle) & SDL_WINDOW_SHOWN; }

	uint WindowSDL::GetWindowVideoDisplayIndex()
	{
		if (handle == nullptr) 
			return 0;
		return SDL_GetWindowDisplayIndex((SDL_Window*)handle);
	}

	DisplayMode WindowSDL::GetWindowDisplayMode()
	{
		if (handle == nullptr)
			return DisplayMode();

		SDL_DisplayMode mode;
		SDL_GetWindowDisplayMode((SDL_Window*)handle, &mode);
		DisplayMode out;

		Result result;
		out.format = BlazeDisplayPixelFormat(mode.format, result);
		out.refreshRate = mode.refresh_rate;
		out.size = Vec2i(mode.w, mode.h);

		return out;
	}
	void WindowSDL::SetWindowDisplayMode(DisplayMode mode)
	{
		if (handle == nullptr)
			return;

		Result result;

		SDL_DisplayMode out;
		out.format = SDLDisplayPixelFormat(mode.format, result);
		out.w = mode.size.x;
		out.h = mode.size.y;
		out.refresh_rate = mode.refreshRate;
		out.driverdata = nullptr;
		SDL_SetWindowDisplayMode((SDL_Window*)handle, &out);
	}			
	
	WindowSDL& WindowSDL::operator=(WindowSDL&& other) noexcept
	{
		handle = other.handle;
		other.handle = nullptr;

		SDL_SetWindowData((SDL_Window*)handle, "Blaze", this);
		return *this;
	}
}