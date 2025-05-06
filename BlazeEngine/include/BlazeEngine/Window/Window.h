#pragma once
#include <mutex>
#include <climits>
#include <cstdint>
#include <BlazeEngineCore/BlazeEngineCoreDefines.h>
#include <BlazeEngineCore/DataStructures/Rect.h>
#include <BlazeEngineCore/DataStructures/StringUTF8.h>
#include <BlazeEngineCore/DataStructures/StringViewUTF8.h>
#include <BlazeEngineCore/Math/Vector.h>
#include <BlazeEngine/BlazeEngineDefines.h>
#include <BlazeEngine/Window/Display.h>
#include <BlazeEngine/Resources/Bitmap/Bitmap.h>
#include <BlazeEngineCore/Event/EventDispatcher.h>
#include <BlazeEngine/Input/Keyboard.h>
#include <BlazeEngine/Input/Mouse.h>

namespace Blaze
{	
	class Window;

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
		Window* parentWindow          = nullptr;
		StringUTF8 title                 = StringUTF8();
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
		EventDispatcher<Keyboard::KeyDownEvent>            keyDownEventDispatcher;
		EventDispatcher<Keyboard::KeyUpEvent>              keyUpEventDispatcher;
		EventDispatcher<Keyboard::TextInputEvent>          textInputEventDispatcher;
		EventDispatcher<Mouse::MouseButtonDownEvent>       mouseButtonDownEventDispatcher;
		EventDispatcher<Mouse::MouseButtonUpEvent>         mouseButtonUpEventDispatcher;
		EventDispatcher<Mouse::MouseMotionEvent>           mouseMotionEventDispatcher;
		EventDispatcher<Mouse::MouseScrollEvent>           mouseScrollEventDispatcher;
		
		Window();
		Window(const Window&) = delete;
		Window(Window&&) noexcept;
		Window(const WindowCreateOptions& createOptions);
		~Window();

		void Destroy();		
		void SwapBuffers(); //TODO change to Present()

		//Window basic properties

		StringUTF8 GetTitle() const;
		void SetTitle(StringViewUTF8 title);
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
		bool GetUtilityWindowFlag() const; //Readonly, set at creation time
		
		//Window graphical properties

		bool GetBorderlessFlag() const;
		void SetBorderlessFlag(bool borderless);
		bool GetHiddenFlag() const;
		void SetHiddenFlag(bool hidden);
		bool GetTransparentFlag() const; //Readonly		
		bool GetHighPixelDensityFlag() const; //Readonly
		WindowPresentMode GetWindowPresentMode() const;
		void SetWindowPresentMode(WindowPresentMode presentMode);
		WindowGraphicsAPI GetGraphicsAPI() const; //Readonly, set at creation time
		void GetMinAndMaxAspectRatio(float& min, float& max);
		void SetMinAndMaxAspectRatio(float min, float max);		
		float GetOpacity() const;
		void SetOpacity(float opacity);
		Vec2u GetMinimumSize() const;
		void SetMinimumSize(Vec2u size);
		Vec2u GetMaximumSize() const;
		void SetMaximumSize(Vec2u size);
		void SetIcon(BitmapView bitmap);
		void Flash();
		float GetContentScale();

		//Window input properties

		Vec2f GetMousePos() const;
		Recti GetMouseConfinmentRectangle() const;
		void SetMouseConfinmentRectangle(Recti rect);
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

		inline void* GetHandle() const { return handle; }
		inline bool IsNullWindow() const { return handle == nullptr; }		

		Window& operator=(const Window&) = delete;
		Window& operator=(Window&&) noexcept;

		static Window* GetWindowFromHandle(void* handle);

		static EventDispatcherInterface<WindowResizedEvent>     GetGlobalWindowResizedEventDispatcher();
		static EventDispatcherInterface<WindowMovedEvent>       GetGlobalWindowMovedEventDispatcher();
		static EventDispatcherInterface<WindowMinimizedEvent>   GetGlobalWindowMinimizedEventDispatcher();
		static EventDispatcherInterface<WindowMaximizedEvent>   GetGlobalWindowMaximizedEventDispatcher();
		static EventDispatcherInterface<WindowFocusGainedEvent> GetGlobalWindowFocusGainedEventDispatcher();
		static EventDispatcherInterface<WindowFocusLostEvent>   GetGlobalWindowFocusLostEventDispatcher();
		static EventDispatcherInterface<WindowCloseEvent>       GetGlobalWindowCloseEventDispatcher();
		static EventDispatcherInterface<WindowMouseEnterEvent>  GetGlobalWindowMouseEnterEventDispatcher();
		static EventDispatcherInterface<WindowMouseLeaveEvent>  GetGlobalWindowMouseLeaveEventDispatcher();
		static EventDispatcherInterface<WindowDestroyedEvent>   GetGlobalWindowDestroyedEventDispatcher();
private:		
		WindowGraphicsAPI graphicsAPI;
		std::mutex mutex;

		void* handle;
		std::condition_variable resizeCV;		
		int resizeState;
		bool waitForSwapBuffersOnResize;

		Vec2i pos;
		Vec2u size;
				
		void HandleMovedEvent(const WindowMovedEvent& event);
		void HandleResizeEvent(const WindowResizedEvent& event);

		friend class BlazeEngineContext;
	};
}