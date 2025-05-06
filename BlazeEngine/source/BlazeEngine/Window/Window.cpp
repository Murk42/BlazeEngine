#include "pch.h"
#include "BlazeEngine/Window/Window.h"
#include "BlazeEngine/Internal/BlazeEngineContext.h"
#include "BlazeEngine/Internal/Libraries/SDL.h"

#include <Windows.h>
#include <SDL3/SDL.h>

namespace Blaze
{		
	constexpr const char* BlazeWindowPointerPropertyName = "BlazeEngineWindow";

	static void SetBlazeWindowPointer(SDL_PropertiesID properties, Window& window)
	{		
		if (!SDL_SetPointerProperty(properties, BlazeWindowPointerPropertyName, &window))
			BLAZE_ENGINE_CORE_FATAL("SDL_SetPointerProperty failed. SDL returned error: \"" + GetSDLError() + "\"");
	}
	static void SetBlazeWindowPointer(Window& window)
	{
		auto properties = SDL_GetWindowProperties((SDL_Window*)window.GetHandle());

		if (properties == NULL)
			BLAZE_ENGINE_CORE_FATAL("SDL_GetWindowProperties failed. SDL returned error: \"" + GetSDLError() + "\"");

		SetBlazeWindowPointer(properties, window);		
	}
	static void RemoveBlazeWindowPointer(SDL_PropertiesID properties)
	{
		if (!SDL_SetPointerProperty(properties, BlazeWindowPointerPropertyName, nullptr))
			BLAZE_ENGINE_CORE_FATAL("SDL_SetPointerProperty failed. SDL returned error: \"" + GetSDLError() + "\"");
	}
	static void RemoveBlazeWindowPointer(Window& window)
	{
		auto properties = SDL_GetWindowProperties((SDL_Window*)window.GetHandle());

		if (properties == NULL)
			BLAZE_ENGINE_CORE_FATAL("SDL_GetWindowProperties failed. SDL returned error: \"" + GetSDLError() + "\"");

		RemoveBlazeWindowPointer(properties);
	}
	static Window* GetBlazeWindowPointer(SDL_Window* window)
	{
		auto properties = SDL_GetWindowProperties(window);

		if (properties == NULL)
			BLAZE_ENGINE_CORE_FATAL("SDL_GetWindowProperties failed. SDL returned error: \"" + GetSDLError() + "\"");

		return (Window*)SDL_GetPointerProperty(properties, BlazeWindowPointerPropertyName, nullptr);
	}

	static HDC GetMonitorHDC(HWND hwnd) 
	{
		// Get the monitor handle (HMONITOR) where the window is located
		HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
		if (!hMonitor) {
			BLAZE_ENGINE_ERROR("Failed to get HMONITOR");
			return NULL;
		}

		// Get monitor information
		MONITORINFOEX monitorInfo;
		monitorInfo.cbSize = sizeof(MONITORINFOEX);
		if (!GetMonitorInfo(hMonitor, &monitorInfo)) {
			BLAZE_ENGINE_ERROR("Failed to get monitor info");
			return NULL;
		}

		// Get an HDC for the monitor
		HDC hdc = CreateDC(monitorInfo.szDevice, monitorInfo.szDevice, NULL, NULL);
		if (!hdc) {
			BLAZE_ENGINE_ERROR("Failed to get HDC for monitor");
			return NULL;
		}

		return hdc;
	}
	static Vec2f GetMonitorDimensions(HDC hdc)
	{
		return Vec2f(GetDeviceCaps(hdc, HORZSIZE), GetDeviceCaps(hdc, VERTSIZE));
	}
	static Vec2f GetWindowMonitorDimensions(const Window& window)
	{
		SDL_Window* Window = (SDL_Window*)window.GetHandle();
		HWND hwnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(Window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);

		HDC hdc = GetMonitorHDC(hwnd);

		Vec2f dimensions = GetMonitorDimensions(hdc);

		ReleaseDC(nullptr, hdc);

		return dimensions;
	}			
		
	Window::Window()
		: handle(nullptr), graphicsAPI(WindowGraphicsAPI::None), resizeState(0), waitForSwapBuffersOnResize(false)
	{								
	}
	Window::Window(const WindowCreateOptions& createOptions)
		: handle(nullptr), graphicsAPI(createOptions.graphicsAPI), resizeState(0), waitForSwapBuffersOnResize(false)
	{		
		struct FunctionParams {
			const WindowCreateOptions& createOptions;
			void*& handle;
			Window& window;
		} functionParams{ createOptions, handle, *this };

		blazeEngineContext.ExecuteTaskOnMainThread([](void* functionParams) -> void {
			const WindowCreateOptions& createOptions = ((FunctionParams*)functionParams)->createOptions;
			void*& handle = ((FunctionParams*)functionParams)->handle;
			Window& window = ((FunctionParams*)functionParams)->window;

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
				BLAZE_ENGINE_ERROR("Invalid WindowOpenMode enum value");
				break;
			}

			switch (createOptions.graphicsAPI)
			{
			case WindowGraphicsAPI::None: break;
			case WindowGraphicsAPI::Metal: metal = true; break;
			case WindowGraphicsAPI::OpenGL: opengl = true; break;
			case WindowGraphicsAPI::Vulkan: vulkan = true; break;
			default:
				BLAZE_ENGINE_ERROR("Invalid WindowGraphicsAPI enum value");
				break;
			}

			Vec2<int64> pos = (Vec2<int64>)createOptions.pos;
			if (pos.x == INT_MAX) pos.x = SDL_WINDOWPOS_CENTERED;
			if (pos.y == INT_MAX) pos.y = SDL_WINDOWPOS_CENTERED;

			SDL_PropertiesID props = SDL_CreateProperties();			

			if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, createOptions.resizable) == false)
				BLAZE_ENGINE_CORE_FATAL("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN failed. SDL returned error: \"" + GetSDLError() + "\"");			
			if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_BORDERLESS_BOOLEAN, createOptions.borderless) == false)
				BLAZE_ENGINE_CORE_FATAL("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_BORDERLESS_BOOLEAN failed. SDL returned error: \"" + GetSDLError() + "\"");			
			if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_MOUSE_GRABBED_BOOLEAN, createOptions.mouseGrabbed) == false)
				BLAZE_ENGINE_CORE_FATAL("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_MOUSE_GRABBED_BOOLEAN failed. SDL returned error: \"" + GetSDLError() + "\"");							
			if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_FOCUSABLE_BOOLEAN, createOptions.focusable) == false)
				BLAZE_ENGINE_CORE_FATAL("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_FOCUSABLE_BOOLEAN failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_HIDDEN_BOOLEAN, createOptions.hidden) == false)
				BLAZE_ENGINE_CORE_FATAL("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_HIDDEN_BOOLEAN failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_TRANSPARENT_BOOLEAN, createOptions.transparent) == false)
				BLAZE_ENGINE_CORE_FATAL("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_TRANSPARENT_BOOLEAN failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN, createOptions.highPixelDensity) == false)
				BLAZE_ENGINE_CORE_FATAL("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_ALWAYS_ON_TOP_BOOLEAN, createOptions.alwaysOnTop) == false)
				BLAZE_ENGINE_CORE_FATAL("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_ALWAYS_ON_TOP_BOOLEAN failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_UTILITY_BOOLEAN, createOptions.utilityWindow) == false)
				BLAZE_ENGINE_CORE_FATAL("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_UTILITY_BOOLEAN failed. SDL returned error: \"" + GetSDLError() + "\"");

			if (SDL_SetPointerProperty(props, SDL_PROP_WINDOW_CREATE_PARENT_POINTER, createOptions.parentWindow == nullptr ? nullptr : createOptions.parentWindow->GetHandle()) == false)
				BLAZE_ENGINE_CORE_FATAL("Calling SDL_SetPointerProperty with SDL_PROP_WINDOW_CREATE_PARENT_POINTER failed. SDL returned error: \"" + GetSDLError() + "\"");

			if (SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, (const char*)createOptions.title.Buffer()) == false)
				BLAZE_ENGINE_CORE_FATAL("Calling SDL_SetStringProperty with SDL_PROP_WINDOW_CREATE_TITLE_STRING failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, pos.x) == false)
				BLAZE_ENGINE_CORE_FATAL("Calling SDL_SetNumberProperty with SDL_PROP_WINDOW_CREATE_X_NUMBER failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, pos.y) == false)
				BLAZE_ENGINE_CORE_FATAL("Calling SDL_SetNumberProperty with SDL_PROP_WINDOW_CREATE_Y_NUMBER failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, createOptions.size.x) == false)
				BLAZE_ENGINE_CORE_FATAL("Calling SDL_SetNumberProperty with SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, createOptions.size.y) == false)
				BLAZE_ENGINE_CORE_FATAL("Calling SDL_SetNumberProperty with SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER failed. SDL returned error: \"" + GetSDLError() + "\"");

			if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN, fullscreen) == false)
				BLAZE_ENGINE_CORE_FATAL("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_MAXIMIZED_BOOLEAN, maximized) == false)
				BLAZE_ENGINE_CORE_FATAL("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_MAXIMIZED_BOOLEAN failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_MINIMIZED_BOOLEAN, minimized) == false)
				BLAZE_ENGINE_CORE_FATAL("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_MINIMIZED_BOOLEAN failed. SDL returned error: \"" + GetSDLError() + "\"");
			
			if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_METAL_BOOLEAN, metal) == false)
				BLAZE_ENGINE_CORE_FATAL("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_METAL_BOOLEAN failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN, opengl) == false)
				BLAZE_ENGINE_CORE_FATAL("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_VULKAN_BOOLEAN, vulkan) == false)
				BLAZE_ENGINE_CORE_FATAL("Calling SDL_SetBooleanProperty with SDL_PROP_WINDOW_CREATE_VULKAN_BOOLEAN failed. SDL returned error: \"" + GetSDLError() + "\"");
					
			handle = SDL_CreateWindowWithProperties(props);

			if (handle == nullptr)
				BLAZE_ENGINE_CORE_FATAL("Failed to create a SDL window. SDL returned error: \"" + GetSDLError() + "\"");
			else
			{
				if (!SDL_GetWindowPosition((SDL_Window*)handle, &window.pos.x, &window.pos.y))
					BLAZE_ENGINE_CORE_FATAL("SDL_GetWindowPosition failed. SDL returned error: \"" + GetSDLError() + "\"");
				if (!SDL_GetWindowSize((SDL_Window*)handle, (int*)&window.size.x, (int*)&window.size.y))
					BLAZE_ENGINE_CORE_FATAL("SDL_GetWindowSize failed. SDL returned error: \"" + GetSDLError() + "\"");

				SetBlazeWindowPointer(window);
			}

			SDL_DestroyProperties(props);
			}, &functionParams);

		if (handle != nullptr)
		{
			{
				std::lock_guard<std::mutex> lk{ blazeEngineContext.windowsMutex };
				blazeEngineContext.windows.Insert(this);
			}
		}
	}
	Window::Window(Window&& other) noexcept :
		handle(other.handle), resizeState(0), graphicsAPI(other.graphicsAPI), waitForSwapBuffersOnResize(other.waitForSwapBuffersOnResize),
		windowResizedEventDispatcher(std::move(windowResizedEventDispatcher)),
		windowMovedEventDispatcher(std::move(windowMovedEventDispatcher)),
		windowMinimizedEventDispatcher(std::move(windowMinimizedEventDispatcher)),
		windowMaximizedEventDispatcher(std::move(windowMaximizedEventDispatcher)),
		windowFocusGainedEventDispatcher(std::move(windowFocusGainedEventDispatcher)),
		windowFocusLostEventDispatcher(std::move(windowFocusLostEventDispatcher)),
		windowCloseEventDispatcher(std::move(windowCloseEventDispatcher)),
		windowMouseEnterEventDispatcher(std::move(windowMouseEnterEventDispatcher)),
		windowMouseLeaveEventDispatcher(std::move(windowMouseLeaveEventDispatcher)),
		windowDestroyedEventDispatcher(std::move(windowDestroyedEventDispatcher)),
		keyDownEventDispatcher(std::move(keyDownEventDispatcher)),
		keyUpEventDispatcher(std::move(keyUpEventDispatcher)),
		textInputEventDispatcher(std::move(textInputEventDispatcher)),
		mouseButtonDownEventDispatcher(std::move(mouseButtonDownEventDispatcher)),
		mouseButtonUpEventDispatcher(std::move(mouseButtonUpEventDispatcher)),
		mouseMotionEventDispatcher(std::move(mouseMotionEventDispatcher)),
		mouseScrollEventDispatcher(std::move(mouseScrollEventDispatcher)),
		pos(other.pos), size(other.size)
	{
		if (handle != nullptr)
		{
			SetBlazeWindowPointer(*this);

			std::lock_guard<std::mutex> lk2{ blazeEngineContext.windowsMutex };
			blazeEngineContext.windows.Erase(&other);
			blazeEngineContext.windows.Insert(this);
		}		

		other.handle = nullptr;

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
		blazeEngineContext.windows.Erase(this);
		blazeEngineContext.ExecuteTaskOnMainThread([](void* handle) -> void { SDL_DestroyWindow((SDL_Window*)handle); },  handle);

		handle = nullptr;
	}
	void Window::SwapBuffers()
	{
		if (handle == nullptr)		
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");			

		switch (graphicsAPI)
		{
		case Blaze::WindowGraphicsAPI::OpenGL: {
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
		case Blaze::WindowGraphicsAPI::Vulkan:
			break;
		case Blaze::WindowGraphicsAPI::Metal:
			break;
		case Blaze::WindowGraphicsAPI::None:
			break;
		default:
			break;
		}
	}
	StringUTF8 Window::GetTitle() const
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");
				
		const char* str = SDL_GetWindowTitle((SDL_Window*)handle);
		uintMem len = strlen(str);
		return StringUTF8((const void*)str, len);
	}
	void Window::SetTitle(StringViewUTF8 title)
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		if (!SDL_SetWindowTitle((SDL_Window*)handle, (const char*)title.Buffer()))
			BLAZE_ENGINE_ERROR("SDL_SetWindowTitle failed. SDL returned error: \"" + GetSDLError() + "\"");
	}
	Vec2i Window::GetPos() const
	{		
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		return pos;
	}
	void Window::SetPos(Vec2i pos)
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		if (pos.x == INT_MAX) pos.x = SDL_WINDOWPOS_CENTERED;
		if (pos.y == INT_MAX) pos.y = SDL_WINDOWPOS_CENTERED;
		
		if (!SDL_SetWindowPosition((SDL_Window*)handle, pos.x, pos.y))
			BLAZE_ENGINE_ERROR("SDL_SetWindowPosition failed. SDL returned error: \"" + GetSDLError() + "\"");
	}
	Vec2u Window::GetSize() const
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		return size;
	}
	void Window::SetSize(Vec2u s)
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");
		
		if (!SDL_SetWindowSize((SDL_Window*)handle, s.x, s.y))
			BLAZE_ENGINE_ERROR("SDL_GetWindowSizeInPixels failed. SDL returned error: \"" + GetSDLError() + "\"");		
	}
	bool Window::GetResizableFlag() const
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		return bool(SDL_GetWindowFlags((SDL_Window*)handle) & SDL_WINDOW_RESIZABLE);				
	}
	void Window::SetResizableFlag(bool resizable)
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		if (!SDL_SetWindowResizable((SDL_Window*)handle, resizable))
			BLAZE_ENGINE_ERROR("SDL_SetWindowResizable failed. SDL returned error: \"" + GetSDLError() + "\"");
	}
	bool Window::GetMouseGrabbedFlag() const
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		return bool(SDL_GetWindowFlags((SDL_Window*)handle) & SDL_WINDOW_MOUSE_GRABBED);
	}
	void Window::SetMouseGrabbedFlag(bool grabbed)
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		if (!SDL_SetWindowMouseGrab((SDL_Window*)handle, grabbed))
			BLAZE_ENGINE_ERROR("SDL_SetWindowMouseGrab failed. SDL returned error: \"" + GetSDLError() + "\"");
	}
	bool Window::GetFocusableFlag() const
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		return !bool(SDL_GetWindowFlags((SDL_Window*)handle) & SDL_WINDOW_NOT_FOCUSABLE);
	}
	void Window::SetFocusableFlag(bool focusable)
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		if (!SDL_SetWindowFocusable((SDL_Window*)handle, focusable))
			BLAZE_ENGINE_ERROR("SDL_SetWindowFocusable failed. SDL returned error: \"" + GetSDLError() + "\"");
	}
	bool Window::GetAlwaysOnTopFlag() const
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		return bool(SDL_GetWindowFlags((SDL_Window*)handle) & SDL_WINDOW_ALWAYS_ON_TOP);
	}
	void Window::SetAlwaysOnTopFlag(bool alwaysOnTop)
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		if (!SDL_SetWindowAlwaysOnTop((SDL_Window*)handle, alwaysOnTop))
			BLAZE_ENGINE_ERROR("SDL_SetWindowAlwaysOnTop failed. SDL returned error: \"" + GetSDLError() + "\"");
	}
	bool Window::GetUtilityWindowFlag() const
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		return bool(SDL_GetWindowFlags((SDL_Window*)handle) & SDL_WINDOW_UTILITY);		
	}
	bool Window::GetBorderlessFlag() const
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		return bool(SDL_GetWindowFlags((SDL_Window*)handle) & SDL_WINDOW_BORDERLESS);
	}
	void Window::SetBorderlessFlag(bool borderless)
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		if (!SDL_SetWindowBordered((SDL_Window*)handle, !borderless))
			BLAZE_ENGINE_ERROR("SDL_SetWindowBordered failed. SDL returned error: \"" + GetSDLError() + "\"");
	}
	bool Window::GetHiddenFlag() const
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		return bool(SDL_GetWindowFlags((SDL_Window*)handle) & SDL_WINDOW_HIDDEN);
	}
	void Window::SetHiddenFlag(bool hidden)
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		if (hidden)
		{
			if (!SDL_HideWindow((SDL_Window*)handle))
				BLAZE_ENGINE_ERROR("SDL_HideWindow failed. SDL returned error: \"" + GetSDLError() + "\"");
		}
		else
			if (!SDL_ShowWindow((SDL_Window*)handle))
				BLAZE_ENGINE_ERROR("SDL_ShowWindow failed. SDL returned error: \"" + GetSDLError() + "\"");
	}
	bool Window::GetTransparentFlag() const
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");
		
		return bool(SDL_GetWindowFlags((SDL_Window*)handle) & SDL_WINDOW_TRANSPARENT);
	}	
	bool Window::GetHighPixelDensityFlag() const
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		return bool(SDL_GetWindowFlags((SDL_Window*)handle) & SDL_WINDOW_HIGH_PIXEL_DENSITY);
	}
	WindowPresentMode Window::GetWindowPresentMode() const
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

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
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");
		
		auto flags = SDL_GetWindowFlags((SDL_Window*)handle);

		if (!bool(flags & SDL_WINDOW_RESIZABLE))
			BLAZE_ENGINE_CORE_WARNING("Window is unresizable but its present mode is being set");

		switch (presentMode)
		{
		case Blaze::WindowPresentMode::Fullscreen:			
			if (!SDL_SetWindowFullscreen((SDL_Window*)handle, true))
				BLAZE_ENGINE_ERROR("SDL_SetWindowFullscreen failed. SDL returned error: \"" + GetSDLError() + "\"");
			break;
		case Blaze::WindowPresentMode::Maximized:
			if (bool(flags & SDL_WINDOW_FULLSCREEN))			
				if (!SDL_SetWindowFullscreen((SDL_Window*)handle, false))
					BLAZE_ENGINE_ERROR("SDL_SetWindowFullscreen failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (!SDL_MaximizeWindow((SDL_Window*)handle))
				BLAZE_ENGINE_ERROR("SDL_MaximizeWindow failed. SDL returned error: \"" + GetSDLError() + "\"");
			break;
		case Blaze::WindowPresentMode::Minimized:
			if (bool(flags & SDL_WINDOW_FULLSCREEN))
				if (!SDL_SetWindowFullscreen((SDL_Window*)handle, false))
					BLAZE_ENGINE_ERROR("SDL_SetWindowFullscreen failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (!SDL_MinimizeWindow((SDL_Window*)handle))
				BLAZE_ENGINE_ERROR("SDL_MinimizeWindow failed. SDL returned error: \"" + GetSDLError() + "\"");
			break;
		case Blaze::WindowPresentMode::Normal:
			if (bool(flags & SDL_WINDOW_FULLSCREEN))
				if (!SDL_SetWindowFullscreen((SDL_Window*)handle, false))
					BLAZE_ENGINE_ERROR("SDL_SetWindowFullscreen failed. SDL returned error: \"" + GetSDLError() + "\"");
			if (!SDL_RestoreWindow((SDL_Window*)handle))
				BLAZE_ENGINE_ERROR("SDL_RestoreWindow failed. SDL returned error: \"" + GetSDLError() + "\"");
			break;
		default:
			break;
		}
	}
	WindowGraphicsAPI Window::GetGraphicsAPI() const
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");
		return graphicsAPI;
	}
	void Window::GetMinAndMaxAspectRatio(float& min, float& max)
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		if (!SDL_GetWindowAspectRatio((SDL_Window*)handle, &min, &max))
			BLAZE_ENGINE_ERROR("SDL_GetWindowAspectRatio failed. SDL returned error: \"" + GetSDLError() + "\"");
	}
	void Window::SetMinAndMaxAspectRatio(float min, float max)
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		if (!SDL_SetWindowAspectRatio((SDL_Window*)handle, min, max))
			BLAZE_ENGINE_ERROR("SDL_SetWindowAspectRatio failed. SDL returned error: \"" + GetSDLError() + "\"");
	}	
	float Window::GetOpacity() const
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		return SDL_GetWindowOpacity((SDL_Window*)handle);		
	}
	void Window::SetOpacity(float opacity)
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		if (!SDL_SetWindowOpacity((SDL_Window*)handle, opacity))
			BLAZE_ENGINE_ERROR("SDL_SetWindowOpacity failed. SDL returned error: \"" + GetSDLError() + "\"");
	}
	Vec2u Window::GetMinimumSize() const
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		Vec2u minimumSize;
		if (!SDL_GetWindowMinimumSize((SDL_Window*)handle, (int*)&minimumSize.x, (int*)&minimumSize.y))
			BLAZE_ENGINE_ERROR("SDL_GetWindowMinimumSize failed. SDL returned error: \"" + GetSDLError() + "\"");
		return minimumSize;
	}
	void Window::SetMinimumSize(Vec2u size)
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		if (!SDL_SetWindowMinimumSize((SDL_Window*)handle, size.x, size.y))
			BLAZE_ENGINE_ERROR("SDL_SetWindowMinimumSize failed. SDL returned error: \"" + GetSDLError() + "\"");
	}
	Vec2u Window::GetMaximumSize() const
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		Vec2u maximumSize;
		if (!SDL_GetWindowMaximumSize((SDL_Window*)handle, (int*)&maximumSize.x, (int*)&maximumSize.y))
			BLAZE_ENGINE_ERROR("SDL_GetWindowMaximumSize failed. SDL returned error: \"" + GetSDLError() + "\"");
		return maximumSize;
	}
	void Window::SetMaximumSize(Vec2u size)
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		if (!SDL_SetWindowMaximumSize((SDL_Window*)handle, size.x, size.y))
			BLAZE_ENGINE_ERROR("SDL_SetWindowMaximumSize failed. SDL returned error: \"" + GetSDLError() + "\"");
	}
	void Window::SetIcon(BitmapView bitmap)
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		if (bitmap.GetPixelType() != BitmapColorComponentType::Uint8 || bitmap.GetPixelFormat() != BitmapColorFormat::RGBA)
		{
			BLAZE_ENGINE_ERROR("Unsupported bitmap type");
			return;
		}
		
		SDL_Surface* surface = SDL_CreateSurfaceFrom(bitmap.GetSize().x, bitmap.GetSize().y, SDL_PIXELFORMAT_RGBA32, (void*)bitmap.GetPixels(), bitmap.GetStride());		
		if (surface == NULL)
		{
			BLAZE_ENGINE_ERROR("SDL_CreateSurfaceFrom failed. SDL returned error: \"" + GetSDLError() + "\"");
			return;
		}
		if (!SDL_SetWindowIcon((SDL_Window*)handle, surface))			
			BLAZE_ENGINE_ERROR("SDL_SetWindowIcon failed. SDL returned error: \"" + GetSDLError() + "\"");

		SDL_DestroySurface(surface);
	}
	Vec2f Window::GetMousePos() const
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		float x, y;
		int wx, wy, wh;

		SDL_GetGlobalMouseState(&x, &y);
		
		if (!SDL_GetWindowPosition((SDL_Window*)handle, &wx, &wy))
		{
			BLAZE_ENGINE_ERROR("SDL_GetWindowPosition failed. SDL returned error: \"" + GetSDLError() + "\"");
			return Vec2f();
		}
		if (!SDL_GetWindowSize((SDL_Window*)handle, nullptr, &wh))
		{
			BLAZE_ENGINE_ERROR("SDL_GetWindowSize failed. SDL returned error: \"" + GetSDLError() + "\"");
			return Vec2f();
		}
		return Vec2f(x - wx, wh - y + wy);
	}
	Recti Window::GetMouseConfinmentRectangle() const
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		auto rect = SDL_GetWindowMouseRect((SDL_Window*)handle);

		if (rect == NULL)
			return Recti();
		else
			return Recti(rect->x, rect->y, rect->w, rect->h);
	}
	void Window::SetMouseConfinmentRectangle(Recti rect)
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		SDL_Rect _rect{
			.x = rect.x,
			.y = rect.y,
			.w = rect.w,
			.h = rect.h
		};

		if (rect.w == 0 || rect.h == 0)
		{
			if (!SDL_SetWindowMouseRect((SDL_Window*)handle, nullptr))
				BLAZE_ENGINE_ERROR("SDL_SetWindowMouseRect failed. SDL returned error: \"" + GetSDLError() + "\"");
		}
		else if (!SDL_SetWindowMouseRect((SDL_Window*)handle, &_rect))
			 BLAZE_ENGINE_ERROR("SDL_SetWindowMouseRect failed. SDL returned error: \"" + GetSDLError() + "\"");
	}
	bool Window::GetRelativeMouseModeFlag() const
	{
		return SDL_GetWindowRelativeMouseMode((SDL_Window*)handle);
	}
	void Window::SetRelativeMouseModeFlag(bool enabled)
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		if (!SDL_SetWindowRelativeMouseMode((SDL_Window*)handle, enabled))
			BLAZE_ENGINE_ERROR("SDL_SetWindowRelativeMouseMode failed. SDL returned error: \"" + GetSDLError() + "\"");
	}
	bool Window::GetKeyboardGrabFlag() const
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		return SDL_GetWindowKeyboardGrab((SDL_Window*)handle);		
	}
	void Window::SetKeyboardGrabFlag(bool grab)
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		if (!SDL_SetWindowKeyboardGrab((SDL_Window*)handle, grab))
			BLAZE_ENGINE_ERROR("SDL_SetWindowKeyboardGrab failed. SDL returned error: \"" + GetSDLError() + "\"");
	}	
	Window* Window::GetParentWindow() const
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		SDL_Window* parent = SDL_GetWindowParent((SDL_Window*)handle);

		if (parent == nullptr)
			return nullptr;

		auto _parent = GetBlazeWindowPointer(parent);

		if (_parent == nullptr)
			BLAZE_ENGINE_CORE_WARNING("GetParentWindow called and the parent window doesnt have the blaze window pointer proeprty set or it is nullptr so nullptr is returned.");

		return _parent;
	}
	Vec2f Window::GetWindowMonitorDimensions() const
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		return Blaze::GetWindowMonitorDimensions(*this);
	}
	void Window::Raise()
	{
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		if (!SDL_RaiseWindow((SDL_Window*)handle))
			BLAZE_ENGINE_ERROR("SDL_RaiseWindow failed. SDL returned error: \"" + GetSDLError() + "\"");
	}
	Display::DisplayID Window::GetDisplayIndex()
	{		
		if (handle == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Window handle is nullptr");

		uint32 id = SDL_GetDisplayForWindow((SDL_Window*)handle);

		if (id == 0)
			BLAZE_ENGINE_ERROR("SDL_GetDisplayForWindow failed. SDL returned error: \"" + GetSDLError() + "\"");

		return id;
	}
	Window& Window::operator=(Window&& other) noexcept
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
		
		handle = other.handle;
		graphicsAPI = other.graphicsAPI;
		other.handle = nullptr;

		pos = other.pos;
		size = other.size;

		SetBlazeWindowPointer(*this);		

		{
			std::lock_guard<std::mutex> lk2{ blazeEngineContext.windowsMutex };
			blazeEngineContext.windows.Erase(&other);
			blazeEngineContext.windows.Insert(this);
		}
		return *this;
	}
	Window* Window::GetWindowFromHandle(void* handle)
	{
		return GetBlazeWindowPointer((SDL_Window*)handle);		
	}	
	void Window::HandleMovedEvent(const WindowMovedEvent& event)
	{
		if (pos == event.pos)
			return;

		blazeEngineContext.eventStack.Add("WindowMovedEvent", [](const WindowMovedEvent& event) {

			{
				std::lock_guard lk{ event.window.mutex };
				event.window.pos = event.pos;
			}

			blazeEngineContext.windowMovedEventDispatcher.Call(event);
			event.window.windowMovedEventDispatcher.Call(event);			
			}, event);
	}
	void Window::HandleResizeEvent(const WindowResizedEvent& event)
	{
		std::unique_lock lk{ mutex };
		resizeState = 0;

		blazeEngineContext.eventStack.Add("WindowResizedEvent", [](const WindowResizedEvent& event) {

			Vec2i oldWindowPos = event.window.pos;
			Vec2i newWindowPos = oldWindowPos + Vec2i(0, event.window.size.y - event.size.y);
			WindowMovedEvent movedEvent{
				.window = event.window,
				.timeNS = event.timeNS,
				.pos = newWindowPos
			};

			{
				std::lock_guard lk{ event.window.mutex };
				event.window.pos = newWindowPos;
				event.window.size = event.size;
			}			

			if (oldWindowPos != newWindowPos)
			{
				blazeEngineContext.windowMovedEventDispatcher.Call(movedEvent);
				event.window.windowMovedEventDispatcher.Call(movedEvent);
			}

			blazeEngineContext.windowResizedEventDispatcher.Call(event);
			event.window.windowResizedEventDispatcher.Call(event);

			{
				std::lock_guard lk{ event.window.mutex };
				event.window.resizeState = 1;
			}
			}, event);

		if (waitForSwapBuffersOnResize)
		{
			//Wait for the event to be handled (mainly to wait for the previous draw and set the viewport) and the window to be redrawn
			resizeCV.wait_for(lk, std::chrono::milliseconds(100), [this]() {
				return this->resizeState == 2;
				});
		}
	}
}