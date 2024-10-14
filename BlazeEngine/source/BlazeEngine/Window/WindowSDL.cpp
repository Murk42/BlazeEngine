#include "pch.h"
#include "BlazeEngine/Window/WindowSDL.h"
#include "BlazeEngine/Internal/Conversions.h"
#include "BlazeEngine/Internal/GlobalData.h"

#include <SDL2/SDL.h>

namespace Blaze
{		
	static StringView GetSDLError()
	{
		const char* ptr = SDL_GetError();
		uintMem len = strlen(ptr);
		return StringView(ptr, len);
	}

	WindowSDL::WindowSDLHandle CreateWindowSDLHandle(WindowSDLCreateOptions createOptions)
	{
		SDL_Window* SDLWindowHandle = nullptr;

		uint32 flags = SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE;
		switch (createOptions.graphicsAPI)
		{
		case WindowSDLGraphicsAPI::Metal:
			flags |= SDL_WINDOW_METAL;
			break;
		case WindowSDLGraphicsAPI::OpenGL:
			flags |= SDL_WINDOW_OPENGL;
			break;
		case WindowSDLGraphicsAPI::Vulkan:
			flags |= SDL_WINDOW_VULKAN;
			break;
		default:
			Debug::Logger::LogError("Blaze Engine", "Invalid WindowSDLGraphicsAPI enum value");
			break;
		}

		switch (createOptions.openMode)
		{
		default:
			Debug::Logger::LogError("Blaze Engine", "Invalid WindowSDLOpenMode enum value");
			break;
		case WindowSDLOpenMode::Normal:
			break;
		case WindowSDLOpenMode::Fullscreen:
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
			break;
		case WindowSDLOpenMode::Maximized:
			flags |= SDL_WINDOW_MAXIMIZED;
			break;
		case WindowSDLOpenMode::Minimized:
			flags |= SDL_WINDOW_MINIMIZED;
			break;			
		}

		if ((bool)(createOptions.styleFlags & WindowSDLStyleFlags::LockMouse))
			flags |= SDL_WINDOW_MOUSE_GRABBED;
		if ((bool)(createOptions.styleFlags & WindowSDLStyleFlags::Resizable))
			flags |= SDL_WINDOW_RESIZABLE;
		if ((bool)(createOptions.styleFlags & WindowSDLStyleFlags::Borderless))
			flags |= SDL_WINDOW_BORDERLESS;

		Vec2i pos = createOptions.pos;
		if (pos.x == INT_MAX) pos.x = SDL_WINDOWPOS_CENTERED;
		if (pos.y == INT_MAX) pos.y = SDL_WINDOWPOS_CENTERED;
		
		blazeEngineContext.ExecuteTaskOnMainThread([&]() {
			SDL_ClearError();

			SDLWindowHandle = SDL_CreateWindow((const char*)createOptions.title.Buffer(), pos.x, pos.y, createOptions.size.x, createOptions.size.y, flags);

			if (SDLWindowHandle == nullptr)			
				Debug::Logger::LogFatal("Blaze Engine", "Failed to create a SDL window. SDL_GetError returned: \"" + GetSDLError() + "\"");

			StringView error = GetSDLError();
			if (!error.Empty())
				Debug::Logger::LogFatal("Blaze Engine", "SDL_CreateWindow set an error.  SDL_GetError returned: \"" + error + "\"");

			});
		
		if (SDLWindowHandle == nullptr)
		{
			Debug::Logger::LogFatal("Blaze Engine", "Failed to create a SDL window. SDL_GetError returned: \"" + GetSDLError() + "\"");
			return (WindowSDL::WindowSDLHandle)nullptr;
		}

		return (WindowSDL::WindowSDLHandle)SDLWindowHandle;
	}
	void DestroyWindowSDLHandle(WindowSDL::WindowSDLHandle handle)
	{
		blazeEngineContext.ExecuteTaskOnMainThread([&]() { SDL_DestroyWindow((SDL_Window*)handle); });
	}
		
	WindowSDL::WindowSDL()
		: handle(nullptr), graphicsAPI(WindowSDLGraphicsAPI::None), resizeState(0)
	{		
	}
	WindowSDL::WindowSDL(WindowSDLCreateOptions createOptions)
		: handle(CreateWindowSDLHandle(createOptions)), graphicsAPI(createOptions.graphicsAPI), resizeState(0)
	{
		//If any events were reported before this line they wont be reported to the client.		
		SDL_SetWindowData((SDL_Window*)handle, "Blaze", this);

	}
	WindowSDL::WindowSDL(WindowSDL&& other) noexcept :		
		WindowBase(std::move(other)), handle(other.handle), resizeState(0), graphicsAPI(other.graphicsAPI)
	{		
		{
			std::lock_guard lk{ other.mutex };
			if (other.resizeState == 1)
				other.resizeState = 0;
		}

		other.handle = nullptr;		
		SDL_SetWindowData((SDL_Window*)handle, "Blaze", this);
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

	void WindowSDL::SwapBuffers()
	{
		switch (graphicsAPI)
		{
		case Blaze::WindowSDLGraphicsAPI::OpenGL: {
			{
				std::lock_guard lk{ mutex };

				SDL_GL_SwapWindow((SDL_Window*)handle);		

				if (resizeState == 1)									
					resizeState = 2;	

				resizeCV.notify_one();

				waitForSwapBuffersOnResize = true;
			}

			break;
		}
		case Blaze::WindowSDLGraphicsAPI::Vulkan:
			break;
		case Blaze::WindowSDLGraphicsAPI::Metal:
			break;
		case Blaze::WindowSDLGraphicsAPI::None:
			break;
		default:
			break;
		}
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
	Vec2u WindowSDL::GetSize() const
	{
		if (handle == nullptr)
			return Vec2u();

		Vec2i size;
		SDL_GetWindowSize((SDL_Window*)handle, &size.x, &size.y);
		return (Vec2u)size;
	}
	StringUTF8 WindowSDL::GetTitle() const
	{
		if (handle == nullptr)
			return "";
		const char* str = SDL_GetWindowTitle((SDL_Window*)handle);
		uintMem len = strlen(str);
		return StringUTF8((const void*)str, len);
	}

	Vec2i WindowSDL::GetMousePos() const
	{
		int x, y, wx, wy, wh;
		SDL_GetGlobalMouseState(&x, &y);
		SDL_GetWindowPosition((SDL_Window*)handle, &wx, &wy);
		SDL_GetWindowSize((SDL_Window*)handle, nullptr, &wh);
		return Vec2i(x - wx, wh - y + wy);
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
	void WindowSDL::SetPos(Vec2i pos)
	{
		if (handle == nullptr)
			return;

		if (pos.x == INT_MAX) pos.x = SDL_WINDOWPOS_CENTERED;
		if (pos.y == INT_MAX) pos.y = SDL_WINDOWPOS_CENTERED;
		
		SDL_SetWindowPosition((SDL_Window*)handle, pos.x, pos.y);
	}
	void WindowSDL::SetSize(Vec2u s)
	{
		if (handle == nullptr)
			return;

		SDL_SetWindowSize((SDL_Window*)handle, s.x, s.y);
	}
	void WindowSDL::SetTitle(StringViewUTF8 title)
	{
		if (handle == nullptr)
			return;

		SDL_SetWindowTitle((SDL_Window*)handle, (const char*)title.Buffer());
	}
	void WindowSDL::SetMinimumSize(Vec2u size)
	{
		if (handle == nullptr)
			return;

		SDL_SetWindowMinimumSize((SDL_Window*)handle, size.x, size.y);
	}
	void WindowSDL::SetMaximumSize(Vec2u size)
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
			SDL_SetWindowFullscreen((SDL_Window*)handle, SDL_WINDOW_FULLSCREEN_DESKTOP);
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

	void WindowSDL::SetWindowLockMouseFlag(bool lockMouse)
	{
		if (handle == nullptr)
			return;

		SDL_SetWindowGrab((SDL_Window*)handle, (SDL_bool)lockMouse);
	}	

	bool WindowSDL::IsFullscreen() const { if (handle == nullptr) return false; return SDL_GetWindowFlags((SDL_Window*)handle) & SDL_WINDOW_FULLSCREEN_DESKTOP; }
	bool WindowSDL::IsBorderless() const { if (handle == nullptr) return false; return SDL_GetWindowFlags((SDL_Window*)handle) & SDL_WINDOW_BORDERLESS; }
	bool WindowSDL::IsResizable() const { if (handle == nullptr) return false; return SDL_GetWindowFlags((SDL_Window*)handle) & SDL_WINDOW_RESIZABLE; }
	bool WindowSDL::IsMouseLocked() const { if (handle == nullptr) return false; return SDL_GetWindowGrab((SDL_Window*)handle); }
	bool WindowSDL::IsMinimized() const { if (handle == nullptr) return false; return SDL_GetWindowFlags((SDL_Window*)handle) & SDL_WINDOW_MINIMIZED; }
	bool WindowSDL::IsMaximized() const { if (handle == nullptr) return false; return SDL_GetWindowFlags((SDL_Window*)handle) & SDL_WINDOW_MAXIMIZED; }
	bool WindowSDL::IsShown() const { if (handle == nullptr) return false; return SDL_GetWindowFlags((SDL_Window*)handle) & SDL_WINDOW_SHOWN; }

	uint WindowSDL::GetWindowVideoDisplayIndex() const
	{
		if (handle == nullptr) 
			return 0;
		return SDL_GetWindowDisplayIndex((SDL_Window*)handle);
	}

	DisplayMode WindowSDL::GetWindowDisplayMode() const
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

	uint WindowSDL::GetDisplayCount()
	{ 
		return SDL_GetNumVideoDisplays();
	}

	uint WindowSDL::GetDisplayModeCount(uint videoDisplayIndex)
	{
		return SDL_GetNumDisplayModes(videoDisplayIndex);
	}

	DisplayMode WindowSDL::GetDisplayMode(uint videoDisplayIndex, uint index)
	{
		Result result;

		SDL_DisplayMode mode;
		SDL_GetDisplayMode(videoDisplayIndex, index, &mode);
		DisplayMode out;

		out.format = BlazeDisplayPixelFormat(mode.format, result);
		out.refreshRate = mode.refresh_rate;
		out.size = Vec2i(mode.w, mode.h);
		return out;
	}
	
	WindowSDL& WindowSDL::operator=(WindowSDL&& other) noexcept
	{
		handle = other.handle;
		graphicsAPI = other.graphicsAPI;
		other.handle = nullptr;

		SDL_SetWindowData((SDL_Window*)handle, "Blaze", this);
		return *this;
	}

	void WindowSDL::WindowResizeFunction(WindowSDL* window)
	{
		Vec2u newWindowSize;

		{
			std::lock_guard lk{ window->mutex };
			newWindowSize = window->latestWindowSize;
		}

		blazeEngineContext.inputEventSystem.windowResizedDispatcher.Call({ window, newWindowSize });
		window->resizedEventDispatcher.Call({ window, newWindowSize });

		{
			std::lock_guard lk{ window->mutex };
			window->resizeState = 1;
		}
	}

	void WindowSDL::HandleResizeEvent(const Input::Events::WindowResizedEvent& event)
	{
		std::unique_lock lk{ mutex };
		resizeState = 0;
		latestWindowSize = event.size;

		blazeEngineContext.eventStack.Add("WindowResizedEvent", WindowResizeFunction, this);

		if (waitForSwapBuffersOnResize)
		{
			//Wait for the event to be handled (mainly to wait for the previous draw and set the viewport) and the window to be redrawn
			resizeCV.wait_for(lk, std::chrono::milliseconds(100), [this]() {
				return this->resizeState == 2;
				});
		}
	}
}