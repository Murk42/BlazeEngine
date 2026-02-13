#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"
#include "BlazeEngineCore/Math/Vector.h"
#include "BlazeEngineCore/String/String.h"
#include "BlazeEngine/Window/Display.h"
#include "BlazeEngine/Common/Bitmap.h"
#include "BlazeEngine/Input/Input.h"
#include "BlazeEngineCore/Event/EventDispatcher.h"
#include <mutex>

namespace Blaze
{
	class Window;
	class BlazeEngineContext;

	enum class WindowGraphicsAPI
	{
		OpenGL,
		Vulkan,
		Metal,
		None
	};
	enum class WindowPresentMode
	{
		Fullscreen, Maximized, Minimized, Normal
	};
	enum class WindowFlashOperation
	{
		FlashBriefly,
		CancelFlash,
		FlashUntilFocused
	};

	//Popup and menu windows are handled seperatelly
	struct WindowCreateOptions
	{
		bool resizable : 1               = true;
		bool borderless : 1              = false;
		bool mouseGrabbed : 1            = false;
		bool focusable : 1               = true;
		bool hidden : 1                  = false;
		bool transparent : 1             = false;
		bool highPixelDensity : 1        = false;
		bool alwaysOnTop : 1             = false;
		bool utilityWindow : 1           = false;
		Window* parentWindow                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   = nullptr;
		u8String title                 = u8String();
		Vec2i pos                        = Vec2i(INT_MAX, INT_MAX);
		Vec2u size                       = Vec2u(960, 540);
		WindowPresentMode presentMode       = WindowPresentMode::Normal;
		WindowGraphicsAPI graphicsAPI = WindowGraphicsAPI::None;
	};

	class BLAZE_API Window
	{
	public:
		struct WindowResizedEvent        { Window& window; uint64 timeNS; Vec2u size; };
		struct WindowMovedEvent          { Window& window; uint64 timeNS; Vec2i pos;  };
		struct WindowMinimizedEvent      { Window& window; uint64 timeNS; };
		struct WindowMaximizedEvent      { Window& window; uint64 timeNS; };
		struct WindowFocusGainedEvent    { Window& window; uint64 timeNS; };
		struct WindowFocusLostEvent      { Window& window; uint64 timeNS; };
		struct WindowCloseEvent          { Window& window; uint64 timeNS; };
		struct WindowMouseEnterEvent     { Window& window; uint64 timeNS; };
		struct WindowMouseLeaveEvent     { Window& window; uint64 timeNS; };
		struct WindowDisplayChangedEvent { Window& window; uint64 timeNS; Display::DisplayID displayID; }; //The displayID is the id of the new display
		struct WindowDestroyedEvent      { Window& window; uint64 timeNS; };

		EventDispatcher<WindowResizedEvent>                windowResizedEventDispatcher;
		EventDispatcher<WindowMovedEvent>                  windowMovedEventDispatcher;
		EventDispatcher<WindowMinimizedEvent>              windowMinimizedEventDispatcher;
		EventDispatcher<WindowMaximizedEvent>              windowMaximizedEventDispatcher;
		EventDispatcher<WindowFocusGainedEvent>            windowFocusGainedEventDispatcher;
		EventDispatcher<WindowFocusLostEvent>              windowFocusLostEventDispatcher;
		EventDispatcher<WindowCloseEvent>                  windowCloseEventDispatcher;
		EventDispatcher<WindowMouseEnterEvent>             windowMouseEnterEventDispatcher;
		EventDispatcher<WindowMouseLeaveEvent>             windowMouseLeaveEventDispatcher;
		EventDispatcher<WindowDisplayChangedEvent>         windowDisplayChangedEventDispatcher;
		EventDispatcher<WindowDestroyedEvent>              windowDestroyedEventDispatcher;
		EventDispatcher<Input::KeyDownEvent>               keyDownEventDispatcher;
		EventDispatcher<Input::KeyUpEvent>                 keyUpEventDispatcher;
		EventDispatcher<Input::TextInputEvent>             textInputEventDispatcher;
		EventDispatcher<Input::MouseButtonDownEvent>       mouseButtonDownEventDispatcher;
		EventDispatcher<Input::MouseButtonUpEvent>         mouseButtonUpEventDispatcher;
		EventDispatcher<Input::MouseMotionEvent>           mouseMotionEventDispatcher;
		EventDispatcher<Input::MouseScrollEvent>           mouseScrollEventDispatcher;

		Window();
		Window(const Window&) = delete;
		Window(Window&&) noexcept;
		Window(const WindowCreateOptions& createOptions);
		Window(const WindowCreateOptions& createOptions, BlazeEngineContext* context);
		Window(void* handle, WindowGraphicsAPI graphicsAPI, BlazeEngineContext* context);
		~Window();

		void Destroy();
		void SwapBuffers(); //TODO change to Present()

		//Window basic properties

		u8String GetTitle() const;
		void SetTitle(u8StringView title);
		Vec2i GetPos() const;
		void SetPos(Vec2i pos);
		Vec2u GetSize() const;
		void SetSize(Vec2u size);

		//Window behaviour properties

		bool GetResizableFlag() const;
		void SetResizableFlag(bool resizable);
		bool GetMouseGrabbedFlag() const;
		void SetMouseGrabbedFlag(bool grabbed);
		bool GetFocusableFlag() const; //TODO this might not exist
		void SetFocusableFlag(bool focusable);
		bool GetAlwaysOnTopFlag() const;
		void SetAlwaysOnTopFlag(bool alwaysOnTop);
		bool GetUtilityWindowFlag() const; //Read-only, set at creation time

		//Window graphical properties

		bool GetBorderlessFlag() const;
		void SetBorderlessFlag(bool borderless);
		bool GetHiddenFlag() const;
		void SetHiddenFlag(bool hidden);
		bool GetTransparentFlag() const; //Read-only
		bool GetHighPixelDensityFlag() const; //Read-only
		WindowPresentMode GetWindowPresentMode() const;
		void SetWindowPresentMode(WindowPresentMode presentMode);
		WindowGraphicsAPI GetGraphicsAPI() const; //Read-only, set at creation time
		void GetMinAndMaxAspectRatio(float& min, float& max);
		void SetMinAndMaxAspectRatio(float min, float max);
		float GetOpacity() const;
		void SetOpacity(float opacity);
		Vec2u GetMinimumSize() const;
		void SetMinimumSize(Vec2u size);
		Vec2u GetMaximumSize() const;
		void SetMaximumSize(Vec2u size);
		void SetIcon(BitmapView bitmap);
		void Flash(WindowFlashOperation flashOperation);
		float GetContentScale();

		//Window input properties

		Vec2f GetMousePos() const;
		Recti GetMouseConfinementRectangle() const;
		void SetMouseConfinementRectangle(Recti rect);
		bool GetRelativeMouseModeFlag() const;
		void SetRelativeMouseModeFlag(bool enabled);
		bool GetKeyboardGrabFlag() const;
		void SetKeyboardGrabFlag(bool grab);

		Window* GetParentWindow() const;
		Vec2f GetWindowMonitorDimensions() const;
		void Raise();

		void SetDisplayModeForFullscreen(const Display::DisplayMode& displayMode);
		Display::DisplayID GetDisplayIndex();

		//GetFullscreenMode() const;
		//SetFullscreenMode();
		//GetWindowVideoDisplayIndex() const ;

		void* ReleaseHandle();
		inline void* GetHandle() const { return handle; }
		inline BlazeEngineContext* GetBlazeEngineContext() const { return context; }

		Window& operator=(const Window&) = delete;
		Window& operator=(Window&&) noexcept;
	private:
		BlazeEngineContext* context;
		WindowGraphicsAPI graphicsAPI;
		void* handle;

		void MoveDispatchers(Window& other);
		static void HandleEvents(void*, void*);
	};
}