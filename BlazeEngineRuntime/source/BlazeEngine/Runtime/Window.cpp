#include "pch.h"
#include "BlazeEngine/Runtime/Window.h"
#include "BlazeEngine/Runtime/BlazeEngineContext.h"
#include "BlazeEngine/Platform/Windows/Windows.h"
#include "BlazeEngine/External/SDL/SDL.h"
#include "BlazeEngine/External/SDL/SDLConversions.h"
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_init.h>

namespace Blaze
{
	Window::Window(const WindowCreateOptions& createOptions)
		: Window(createOptions, blazeEngineContext)
	{

	}
	Window::Window(const WindowCreateOptions& createOptions, BlazeEngineContext* context)
		: graphicsAPI(WindowGraphicsAPI::None), handle(nullptr), context(context)
	{
		bool fullscreen = false;
		bool maximized = false;
		bool minimized = false;

		bool metal = false;
		bool opengl = false;
		bool vulkan = false;

		switch (createOptions.presentMode)
		{
		case WindowPresentMode::Normal: break;
		case WindowPresentMode::Fullscreen:	fullscreen = true; break;
		case WindowPresentMode::Maximized: maximized = true; break;
		case WindowPresentMode::Minimized: minimized = true; break;
		default:
			BLAZE_LOG_ERROR("Invalid WindowOpenMode enum value");
			break;
		}

		switch (createOptions.graphicsAPI)
		{
		case WindowGraphicsAPI::None: break;
		case WindowGraphicsAPI::Metal: metal = true; break;
		case WindowGraphicsAPI::OpenGL: opengl = true; break;
		case WindowGraphicsAPI::Vulkan: vulkan = true; break;
		default:
			BLAZE_LOG_ERROR("Invalid WindowGraphicsAPI enum value");
			break;
		}

		Vec2<int64> pos = (Vec2<int64>)createOptions.pos;
		if (pos.x == INT_MAX) pos.x = SDL_WINDOWPOS_CENTERED;
		if (pos.y == INT_MAX) pos.y = SDL_WINDOWPOS_CENTERED;

		SDL_PropertiesID properties = SDL_CreateProperties();

		if (SDL_SetBooleanProperty(properties, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, createOptions.resizable) == false)
			BLAZE_LOG_FATAL("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetBooleanProperty(properties, SDL_PROP_WINDOW_CREATE_BORDERLESS_BOOLEAN, createOptions.borderless) == false)
			BLAZE_LOG_FATAL("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_BORDERLESS_BOOLEAN failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetBooleanProperty(properties, SDL_PROP_WINDOW_CREATE_MOUSE_GRABBED_BOOLEAN, createOptions.mouseGrabbed) == false)
			BLAZE_LOG_FATAL("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_MOUSE_GRABBED_BOOLEAN failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetBooleanProperty(properties, SDL_PROP_WINDOW_CREATE_FOCUSABLE_BOOLEAN, createOptions.focusable) == false)
			BLAZE_LOG_FATAL("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_FOCUSABLE_BOOLEAN failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetBooleanProperty(properties, SDL_PROP_WINDOW_CREATE_HIDDEN_BOOLEAN, createOptions.hidden) == false)
			BLAZE_LOG_FATAL("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_HIDDEN_BOOLEAN failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetBooleanProperty(properties, SDL_PROP_WINDOW_CREATE_TRANSPARENT_BOOLEAN, createOptions.transparent) == false)
			BLAZE_LOG_FATAL("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_TRANSPARENT_BOOLEAN failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetBooleanProperty(properties, SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN, createOptions.highPixelDensity) == false)
			BLAZE_LOG_FATAL("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetBooleanProperty(properties, SDL_PROP_WINDOW_CREATE_ALWAYS_ON_TOP_BOOLEAN, createOptions.alwaysOnTop) == false)
			BLAZE_LOG_FATAL("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_ALWAYS_ON_TOP_BOOLEAN failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetBooleanProperty(properties, SDL_PROP_WINDOW_CREATE_UTILITY_BOOLEAN, createOptions.utilityWindow) == false)
			BLAZE_LOG_FATAL("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_UTILITY_BOOLEAN failed. SDL returned error: \"" + SDL_GetError() + "\"");

		if (SDL_SetPointerProperty(properties, SDL_PROP_WINDOW_CREATE_PARENT_POINTER, createOptions.parentWindow == nullptr ? nullptr : createOptions.parentWindow->GetHandle()) == false)
			BLAZE_LOG_FATAL("Calling SDL_SetPointerProperty with SDL_PROP_WINDOW_CREATE_PARENT_POINTER failed. SDL returned error: \"" + SDL_GetError() + "\"");

		if (SDL_SetStringProperty(properties, SDL_PROP_WINDOW_CREATE_TITLE_STRING, reinterpret_cast<const char*>(createOptions.title.Ptr())) == false)
			BLAZE_LOG_FATAL("Calling SDL_SetStringProperty with SDL_PROP_WINDOW_CREATE_TITLE_STRING failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetNumberProperty(properties, SDL_PROP_WINDOW_CREATE_X_NUMBER, pos.x) == false)
			BLAZE_LOG_FATAL("Calling SDL_SetNumberProperty with SDL_PROP_WINDOW_CREATE_X_NUMBER failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetNumberProperty(properties, SDL_PROP_WINDOW_CREATE_Y_NUMBER, pos.y) == false)
			BLAZE_LOG_FATAL("Calling SDL_SetNumberProperty with SDL_PROP_WINDOW_CREATE_Y_NUMBER failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetNumberProperty(properties, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, createOptions.size.x) == false)
			BLAZE_LOG_FATAL("Calling SDL_SetNumberProperty with SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetNumberProperty(properties, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, createOptions.size.y) == false)
			BLAZE_LOG_FATAL("Calling SDL_SetNumberProperty with SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER failed. SDL returned error: \"" + SDL_GetError() + "\"");

		if (SDL_SetBooleanProperty(properties, SDL_PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN, fullscreen) == false)
			BLAZE_LOG_FATAL("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetBooleanProperty(properties, SDL_PROP_WINDOW_CREATE_MAXIMIZED_BOOLEAN, maximized) == false)
			BLAZE_LOG_FATAL("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_MAXIMIZED_BOOLEAN failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetBooleanProperty(properties, SDL_PROP_WINDOW_CREATE_MINIMIZED_BOOLEAN, minimized) == false)
			BLAZE_LOG_FATAL("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_MINIMIZED_BOOLEAN failed. SDL returned error: \"" + SDL_GetError() + "\"");

		if (SDL_SetBooleanProperty(properties, SDL_PROP_WINDOW_CREATE_METAL_BOOLEAN, metal) == false)
			BLAZE_LOG_FATAL("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_METAL_BOOLEAN failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetBooleanProperty(properties, SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN, opengl) == false)
			BLAZE_LOG_FATAL("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN failed. SDL returned error: \"" + SDL_GetError() + "\"");
		if (SDL_SetBooleanProperty(properties, SDL_PROP_WINDOW_CREATE_VULKAN_BOOLEAN, vulkan) == false)
			BLAZE_LOG_FATAL("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_VULKAN_BOOLEAN failed. SDL returned error: \"" + SDL_GetError() + "\"");


		struct FunctionParams {
			SDL_PropertiesID properties;
			SDL_Window** handle;
		};

		FunctionParams functionParams{
			properties,
			(SDL_Window**)&handle
		};

		auto function = [](void* functionParams) -> void {
			*((FunctionParams*)functionParams)->handle = SDL_CreateWindowWithProperties(((FunctionParams*)functionParams)->properties);
			};

		if (context == nullptr)
			if (SDL_IsMainThread())
				function(&functionParams);
			else
				BLAZE_LOG_FATAL("Creating a windows on a non-main thread");
		else
		{
			uint64 timeoutMilliseconds = 2000;
			if (!context->ExecuteOnMainThread(function, &functionParams, timeoutMilliseconds))
				BLAZE_LOG_FATAL("Failed to create a SDL window on the main thread in {}ms", timeoutMilliseconds);
		}

		SDL_DestroyProperties(properties);

		if (handle == nullptr)
			BLAZE_LOG_FATAL("Failed to create a SDL window. SDL returned error: \"" + SDL_GetError() + "\"");

		context->RegisterWindow(*this);

		graphicsAPI = createOptions.graphicsAPI;
	}
	Window::Window(void* handle, WindowGraphicsAPI graphicsAPI, BlazeEngineContext* context)
		: handle(handle), graphicsAPI(graphicsAPI), context(context)
	{
		if (context != nullptr)
			context->RegisterWindow(*this);
	}
	Window::Window()
		: handle(nullptr), graphicsAPI(WindowGraphicsAPI::None)
	{
	}
	Window::Window(Window&& other) noexcept :
		handle(other.handle), graphicsAPI(other.graphicsAPI), context(other.context)
	{
		MoveDispatchers(other);
		other.handle = nullptr;
		other.graphicsAPI = WindowGraphicsAPI::None;
		other.context = nullptr;

		context->MoveWindow(*this);
	}
	Window::~Window()
	{
		Destroy();
	}
	void Window::Destroy()
	{
		if (handle == nullptr)
			return;

		windowDestroyedEventDispatcher.Call({ *this });


		if (context == nullptr)
			if (SDL_IsMainThread())
				SDL_DestroyWindow((SDL_Window*)handle);
			else
				BLAZE_LOG_FATAL("Destroying a windows on a non-main thread");
		else
		{
			context->UnregisterWindow(*this);

			uint64 timeoutMilliseconds = 2000;
			if (!context->ExecuteOnMainThread([](void* handle) -> void {
				SDL_DestroyWindow((SDL_Window*)handle);
				}, handle, timeoutMilliseconds))
				BLAZE_LOG_FATAL("Failed to destroy a SDL window on the main thread in {}ms", timeoutMilliseconds);
		}

		handle = nullptr;
	}
	u8String Window::GetTitle() const
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		const char* str = SDL_GetWindowTitle((SDL_Window*)handle);
		uintMem len = strlen(str);
		return u8String(str, len);
	}
	void Window::SetTitle(u8StringView title)
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		if (!SDL_SetWindowTitle((SDL_Window*)handle, reinterpret_cast<const char*>(u8String(title).Ptr())))
			BLAZE_LOG_ERROR("SDL_SetWindowTitle failed. SDL returned error: \"{}\"", SDL_GetError());
	}
	Vec2i Window::GetPos() const
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		Vec2i pos{ };
		if (!SDL_GetWindowPosition((SDL_Window*)handle, &pos.x, &pos.y))
			BLAZE_LOG_ERROR("SDL_GetWindowPosition failed. SDL returned error: \"" + SDL_GetError() + "\"");

		return { pos.x, pos.y };
	}
	void Window::SetPos(Vec2i pos)
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		if (pos.x == INT_MAX) pos.x = SDL_WINDOWPOS_CENTERED;
		if (pos.y == INT_MAX) pos.y = SDL_WINDOWPOS_CENTERED;

		if (!SDL_SetWindowPosition((SDL_Window*)handle, pos.x, pos.y))
			BLAZE_LOG_ERROR("SDL_SetWindowPosition failed. SDL returned error: \"" + SDL_GetError() + "\"");
	}
	Vec2u Window::GetSize() const
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		Vec2u size{ };
		if (!SDL_GetWindowSize((SDL_Window*)handle, reinterpret_cast<int*>(&size.x), reinterpret_cast<int*>(&size.y)))
			BLAZE_LOG_ERROR("SDL_GetWindowSize failed. SDL returned error: \"" + SDL_GetError() + "\"");

		return size;
	}
	void Window::SetSize(Vec2u s)
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		if (!SDL_SetWindowSize((SDL_Window*)handle, s.x, s.y))
			BLAZE_LOG_ERROR("SDL_GetWindowSizeInPixels failed. SDL returned error: \"" + SDL_GetError() + "\"");
	}
	bool Window::GetResizableFlag() const
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		return bool(SDL_GetWindowFlags((SDL_Window*)handle) & SDL_WINDOW_RESIZABLE);
	}
	void Window::SetResizableFlag(bool resizable)
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		if (!SDL_SetWindowResizable((SDL_Window*)handle, resizable))
			BLAZE_LOG_ERROR("SDL_SetWindowResizable failed. SDL returned error: \"" + SDL_GetError() + "\"");
	}
	bool Window::GetMouseGrabbedFlag() const
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		return bool(SDL_GetWindowFlags((SDL_Window*)handle) & SDL_WINDOW_MOUSE_GRABBED);
	}
	void Window::SetMouseGrabbedFlag(bool grabbed)
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		if (!SDL_SetWindowMouseGrab((SDL_Window*)handle, grabbed))
			BLAZE_LOG_ERROR("SDL_SetWindowMouseGrab failed. SDL returned error: \"" + SDL_GetError() + "\"");
	}
	bool Window::GetFocusableFlag() const
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		return !bool(SDL_GetWindowFlags((SDL_Window*)handle) & SDL_WINDOW_NOT_FOCUSABLE);
	}
	void Window::SetFocusableFlag(bool focusable)
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		if (!SDL_SetWindowFocusable((SDL_Window*)handle, focusable))
			BLAZE_LOG_ERROR("SDL_SetWindowFocusable failed. SDL returned error: \"" + SDL_GetError() + "\"");
	}
	bool Window::GetAlwaysOnTopFlag() const
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		return bool(SDL_GetWindowFlags((SDL_Window*)handle) & SDL_WINDOW_ALWAYS_ON_TOP);
	}
	void Window::SetAlwaysOnTopFlag(bool alwaysOnTop)
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		if (!SDL_SetWindowAlwaysOnTop((SDL_Window*)handle, alwaysOnTop))
			BLAZE_LOG_ERROR("SDL_SetWindowAlwaysOnTop failed. SDL returned error: \"" + SDL_GetError() + "\"");
	}
	bool Window::GetUtilityWindowFlag() const
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		return bool(SDL_GetWindowFlags((SDL_Window*)handle) & SDL_WINDOW_UTILITY);
	}
	bool Window::GetBorderlessFlag() const
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		return bool(SDL_GetWindowFlags((SDL_Window*)handle) & SDL_WINDOW_BORDERLESS);
	}
	void Window::SetBorderlessFlag(bool borderless)
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		if (!SDL_SetWindowBordered((SDL_Window*)handle, !borderless))
			BLAZE_LOG_ERROR("SDL_SetWindowBordered failed. SDL returned error: \"" + SDL_GetError() + "\"");
	}
	bool Window::GetHiddenFlag() const
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		return bool(SDL_GetWindowFlags((SDL_Window*)handle) & SDL_WINDOW_HIDDEN);
	}
	void Window::SetHiddenFlag(bool hidden)
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		if (hidden)
		{
			if (!SDL_HideWindow((SDL_Window*)handle))
				BLAZE_LOG_ERROR("SDL_HideWindow failed. SDL returned error: \"" + SDL_GetError() + "\"");
		}
		else
			if (!SDL_ShowWindow((SDL_Window*)handle))
				BLAZE_LOG_ERROR("SDL_ShowWindow failed. SDL returned error: \"" + SDL_GetError() + "\"");
	}
	bool Window::GetTransparentFlag() const
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		return bool(SDL_GetWindowFlags((SDL_Window*)handle) & SDL_WINDOW_TRANSPARENT);
	}
	bool Window::GetHighPixelDensityFlag() const
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		return bool(SDL_GetWindowFlags((SDL_Window*)handle) & SDL_WINDOW_HIGH_PIXEL_DENSITY);
	}
	WindowPresentMode Window::GetWindowPresentMode() const
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		auto flags = SDL_GetWindowFlags((SDL_Window*)handle);

		if (flags & SDL_WINDOW_MAXIMIZED)
			return WindowPresentMode::Maximized;
		else if (flags & SDL_WINDOW_MINIMIZED)
			return WindowPresentMode::Minimized;
		else if (flags & SDL_WINDOW_FULLSCREEN)
			return WindowPresentMode::Fullscreen;
		return WindowPresentMode::Normal;
	}
	void Window::SetWindowPresentMode(WindowPresentMode presentMode)
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		auto flags = SDL_GetWindowFlags((SDL_Window*)handle);

		if (!bool(flags & SDL_WINDOW_RESIZABLE))
			BLAZE_LOG_WARNING("Window is unresizable but its present mode is being set");

		switch (presentMode)
		{
		case Blaze::WindowPresentMode::Fullscreen:
			if (!SDL_SetWindowFullscreen((SDL_Window*)handle, true))
				BLAZE_LOG_ERROR("SDL_SetWindowFullscreen failed. SDL returned error: \"" + SDL_GetError() + "\"");
			break;
		case Blaze::WindowPresentMode::Maximized:
			if (bool(flags & SDL_WINDOW_FULLSCREEN))
				if (!SDL_SetWindowFullscreen((SDL_Window*)handle, false))
					BLAZE_LOG_ERROR("SDL_SetWindowFullscreen failed. SDL returned error: \"" + SDL_GetError() + "\"");
			if (!SDL_MaximizeWindow((SDL_Window*)handle))
				BLAZE_LOG_ERROR("SDL_MaximizeWindow failed. SDL returned error: \"" + SDL_GetError() + "\"");
			break;
		case Blaze::WindowPresentMode::Minimized:
			if (bool(flags & SDL_WINDOW_FULLSCREEN))
				if (!SDL_SetWindowFullscreen((SDL_Window*)handle, false))
					BLAZE_LOG_ERROR("SDL_SetWindowFullscreen failed. SDL returned error: \"" + SDL_GetError() + "\"");
			if (!SDL_MinimizeWindow((SDL_Window*)handle))
				BLAZE_LOG_ERROR("SDL_MinimizeWindow failed. SDL returned error: \"" + SDL_GetError() + "\"");
			break;
		case Blaze::WindowPresentMode::Normal:
			if (bool(flags & SDL_WINDOW_FULLSCREEN))
				if (!SDL_SetWindowFullscreen((SDL_Window*)handle, false))
					BLAZE_LOG_ERROR("SDL_SetWindowFullscreen failed. SDL returned error: \"" + SDL_GetError() + "\"");
			if (!SDL_RestoreWindow((SDL_Window*)handle))
				BLAZE_LOG_ERROR("SDL_RestoreWindow failed. SDL returned error: \"" + SDL_GetError() + "\"");
			break;
		default:
			break;
		}
	}
	WindowGraphicsAPI Window::GetGraphicsAPI() const
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");
		return graphicsAPI;
	}
	void Window::GetMinAndMaxAspectRatio(float& min, float& max)
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		if (!SDL_GetWindowAspectRatio((SDL_Window*)handle, &min, &max))
			BLAZE_LOG_ERROR("SDL_GetWindowAspectRatio failed. SDL returned error: \"" + SDL_GetError() + "\"");
	}
	void Window::SetMinAndMaxAspectRatio(float min, float max)
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		if (!SDL_SetWindowAspectRatio((SDL_Window*)handle, min, max))
			BLAZE_LOG_ERROR("SDL_SetWindowAspectRatio failed. SDL returned error: \"" + SDL_GetError() + "\"");
	}
	float Window::GetOpacity() const
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		return SDL_GetWindowOpacity((SDL_Window*)handle);
	}
	void Window::SetOpacity(float opacity)
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		if (!SDL_SetWindowOpacity((SDL_Window*)handle, opacity))
			BLAZE_LOG_ERROR("SDL_SetWindowOpacity failed. SDL returned error: \"" + SDL_GetError() + "\"");
	}
	Vec2u Window::GetMinimumSize() const
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		Vec2u minimumSize;
		if (!SDL_GetWindowMinimumSize((SDL_Window*)handle, (int*)&minimumSize.x, (int*)&minimumSize.y))
			BLAZE_LOG_ERROR("SDL_GetWindowMinimumSize failed. SDL returned error: \"" + SDL_GetError() + "\"");
		return minimumSize;
	}
	void Window::SetMinimumSize(Vec2u size)
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		if (!SDL_SetWindowMinimumSize((SDL_Window*)handle, size.x, size.y))
			BLAZE_LOG_ERROR("SDL_SetWindowMinimumSize failed. SDL returned error: \"" + SDL_GetError() + "\"");
	}
	Vec2u Window::GetMaximumSize() const
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		Vec2u maximumSize;
		if (!SDL_GetWindowMaximumSize((SDL_Window*)handle, (int*)&maximumSize.x, (int*)&maximumSize.y))
			BLAZE_LOG_ERROR("SDL_GetWindowMaximumSize failed. SDL returned error: \"" + SDL_GetError() + "\"");
		return maximumSize;
	}
	void Window::SetMaximumSize(Vec2u size)
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		if (!SDL_SetWindowMaximumSize((SDL_Window*)handle, size.x, size.y))
			BLAZE_LOG_ERROR("SDL_SetWindowMaximumSize failed. SDL returned error: \"" + SDL_GetError() + "\"");
	}
	void Window::SetIcon(BitmapView bitmap)
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		if (bitmap.GetPixelType() != BitmapColorComponentType::Uint8 || bitmap.GetPixelFormat() != BitmapColorFormat::RGBA)
		{
			BLAZE_LOG_ERROR("Unsupported bitmap type");
			return;
		}

		SDL_Surface* surface = SDL_CreateSurfaceFrom(bitmap.GetSize().x, bitmap.GetSize().y, SDL_PIXELFORMAT_RGBA32, (void*)bitmap.GetPixels(), static_cast<int>(bitmap.GetStride()));
		if (surface == NULL)
		{
			BLAZE_LOG_ERROR("SDL_CreateSurfaceFrom failed. SDL returned error: \"" + SDL_GetError() + "\"");
			return;
		}
		if (!SDL_SetWindowIcon((SDL_Window*)handle, surface))
			BLAZE_LOG_ERROR("SDL_SetWindowIcon failed. SDL returned error: \"" + SDL_GetError() + "\"");

		SDL_DestroySurface(surface);
	}
	void Window::Flash(WindowFlashOperation flashOperation)
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		SDL_FlashOperation _flashOperation = SDL_FLASH_BRIEFLY;
		switch (flashOperation)
		{
		case Blaze::WindowFlashOperation::FlashBriefly: _flashOperation = SDL_FLASH_BRIEFLY; break;
		case Blaze::WindowFlashOperation::CancelFlash: _flashOperation = SDL_FLASH_CANCEL; break;
		case Blaze::WindowFlashOperation::FlashUntilFocused: _flashOperation = SDL_FLASH_UNTIL_FOCUSED; break;
		}

		if (!SDL_FlashWindow((SDL_Window*)handle, _flashOperation))
			BLAZE_LOG_ERROR("SDL_FlashWindow failed. SDL returned error: \"{}\"", SDL_GetError());
	}
	Vec2f Window::GetMousePos() const
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		float x, y;
		int wx, wy, wh;

		SDL_GetGlobalMouseState(&x, &y);

		if (!SDL_GetWindowPosition((SDL_Window*)handle, &wx, &wy))
		{
			BLAZE_LOG_ERROR("SDL_GetWindowPosition failed. SDL returned error: \"" + SDL_GetError() + "\"");
			return Vec2f();
		}
		if (!SDL_GetWindowSize((SDL_Window*)handle, nullptr, &wh))
		{
			BLAZE_LOG_ERROR("SDL_GetWindowSize failed. SDL returned error: \"" + SDL_GetError() + "\"");
			return Vec2f();
		}
		return Vec2f(x - wx, wh - y + wy);
	}
	Recti Window::GetMouseConfinementRectangle() const
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		auto rect = SDL_GetWindowMouseRect((SDL_Window*)handle);

		if (rect == NULL)
			return Recti();
		else
			return Recti(rect->x, rect->y, rect->w, rect->h);
	}
	void Window::SetMouseConfinementRectangle(Recti rect)
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		SDL_Rect _rect{
			.x = rect.x,
			.y = rect.y,
			.w = rect.w,
			.h = rect.h
		};

		if (rect.w == 0 || rect.h == 0)
		{
			if (!SDL_SetWindowMouseRect((SDL_Window*)handle, nullptr))
				BLAZE_LOG_ERROR("SDL_SetWindowMouseRect failed. SDL returned error: \"" + SDL_GetError() + "\"");
		}
		else if (!SDL_SetWindowMouseRect((SDL_Window*)handle, &_rect))
			 BLAZE_LOG_ERROR("SDL_SetWindowMouseRect failed. SDL returned error: \"" + SDL_GetError() + "\"");
	}
	bool Window::GetRelativeMouseModeFlag() const
	{
		return SDL_GetWindowRelativeMouseMode((SDL_Window*)handle);
	}
	void Window::SetRelativeMouseModeFlag(bool enabledFlag)
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		if (!SDL_SetWindowRelativeMouseMode((SDL_Window*)handle, enabledFlag))
			BLAZE_LOG_ERROR("SDL_SetWindowRelativeMouseMode failed. SDL returned error: \"" + SDL_GetError() + "\"");
	}
	bool Window::GetKeyboardGrabFlag() const
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		return SDL_GetWindowKeyboardGrab((SDL_Window*)handle);
	}
	void Window::SetKeyboardGrabFlag(bool grab)
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		if (!SDL_SetWindowKeyboardGrab((SDL_Window*)handle, grab))
			BLAZE_LOG_ERROR("SDL_SetWindowKeyboardGrab failed. SDL returned error: \"" + SDL_GetError() + "\"");
	}
	Window* Window::GetParentWindow() const
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		if (context == nullptr)
			return nullptr;

		SDL_Window* parent = SDL_GetWindowParent((SDL_Window*)handle);

		if (parent == nullptr)
			return nullptr;

		return context->GetWindowFromHandle(parent);
	}
	Vec2u Window::GetWindowMonitorDimensions() const
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		return Blaze::GetWindowMonitorDimensions(*this);
	}
	void Window::Raise()
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		if (!SDL_RaiseWindow((SDL_Window*)handle))
			BLAZE_LOG_ERROR("SDL_RaiseWindow failed. SDL returned error: \"" + SDL_GetError() + "\"");
	}
	void Window::SetDisplayModeForFullscreen(const Display::DisplayMode& displayMode)
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		SDL_DisplayMode _displayMode = SDL_GetSDLDisplayMode(displayMode);

		if (!SDL_SetWindowFullscreenMode((SDL_Window*)handle, &_displayMode))
			BLAZE_LOG_ERROR("SDL_SetWindowFullscreenMode failed. SDL returned error: \"" + SDL_GetError() + "\"");
	}
	Display::DisplayID Window::GetDisplayIndex()
	{
		if (handle == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		uint32 id = SDL_GetDisplayForWindow((SDL_Window*)handle);

		if (id == 0)
			BLAZE_LOG_ERROR("SDL_GetDisplayForWindow failed. SDL returned error: \"" + SDL_GetError() + "\"");

		return id;
	}
	void* Window::ReleaseHandle()
	{
		void* _handle = handle;

		handle = nullptr;
		graphicsAPI = WindowGraphicsAPI::None;
		context = nullptr;

		return _handle;
	}
	Window& Window::operator=(Window&& other) noexcept
	{
		Destroy();

		MoveDispatchers(other);
		handle = other.handle;
		graphicsAPI = other.graphicsAPI;
		context = other.context;
		other.handle = nullptr;
		other.graphicsAPI = WindowGraphicsAPI::None;
		other.context = nullptr;

		if (context != nullptr)
			context->MoveWindow(*this);

		return *this;
	}
	void Window::MoveDispatchers(Window& other)
	{
		windowResizedEventDispatcher = std::move(other.windowResizedEventDispatcher);
		windowMovedEventDispatcher = std::move(other.windowMovedEventDispatcher);
		windowMinimizedEventDispatcher = std::move(other.windowMinimizedEventDispatcher);
		windowMaximizedEventDispatcher = std::move(other.windowMaximizedEventDispatcher);
		windowFocusGainedEventDispatcher = std::move(other.windowFocusGainedEventDispatcher);
		windowFocusLostEventDispatcher = std::move(other.windowFocusLostEventDispatcher);
		windowCloseEventDispatcher = std::move(other.windowCloseEventDispatcher);
		windowMouseEnterEventDispatcher = std::move(other.windowMouseEnterEventDispatcher);
		windowMouseLeaveEventDispatcher = std::move(other.windowMouseLeaveEventDispatcher);
		keyDownEventDispatcher = std::move(other.keyDownEventDispatcher);
		keyUpEventDispatcher = std::move(other.keyUpEventDispatcher);
		mouseMotionEventDispatcher = std::move(other.mouseMotionEventDispatcher);
		mouseScrollEventDispatcher = std::move(other.mouseScrollEventDispatcher);
		textInputEventDispatcher = std::move(other.textInputEventDispatcher);
	}
}