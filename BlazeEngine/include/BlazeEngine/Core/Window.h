#pragma once
#include "BlazeEngine/Resources/Bitmap/Bitmap.h"
#include "BlazeEngine/Event/EventDispatcher.h"
#include "BlazeEngine/Input/InputEvents.h"

namespace Blaze
{
	enum class DisplayPixelFormat
	{
		Any,
		RGB332,
		RGB444,
		RGB555,
		BGR555,
		ARGB4444,
		RGBA4444,
		ABGR4444,
		BGRA4444,
		ARGB1555,
		RGBA5551,
		ABGR1555,
		BGRA5551,
		RGB565,
		BGR565,
		RGB24,
		BGR24,
		RGB888,
		RGBX8888,
		BGR888,
		BGRX8888,
		ARGB8888,
		RGBA8888,
		ABGR8888,
		BGRA8888,
		ARGB2101010
	};

	struct DisplayMode
	{
		DisplayPixelFormat format = DisplayPixelFormat::RGB24;
		Vec2i size = Vec2i(1920, 1080);
		uint refreshRate = 60;
	};

	typedef struct _WindowHandle* WindowHandle;

	enum class WindowHandleGraphicsaAPI
	{
		OpenGL,
		Vulkan,
		Metal,
		None
	};

	/// <summary>
	/// Creates a SDL window handle and returns it.
	/// </summary>
	/// <param name="title">The title of the new window.</param>
	/// <param name="pos">Position of the new window. If a component value is INT_MAX then it will be centered to the display for that component.</param>
	/// <param name="size">Size of the new window</param>
	/// <param name="graphicsAPI">The supported graphics API for the window</param>
	/// <returns>Returns a handle to the SDL window. It can be directly casted to a SDL_Window* </returns>
	BLAZE_API WindowHandle CreateWindowHandle(StringView title, Vec2i pos, Vec2i size, WindowHandleGraphicsaAPI graphicsAPI);	
	BLAZE_API void DestroyWindowHandle(WindowHandle handle);
		 
	class BLAZE_API Window
	{		
	public:				
		EventDispatcher<Input::Events::WindowResizedEvent>     resizedEventDispatcher;
		EventDispatcher<Input::Events::WindowMovedEvent>       movedEventDispatcher;
		EventDispatcher<Input::Events::WindowMinimizedEvent>   minimizedEventDispatcher;
		EventDispatcher<Input::Events::WindowMaximizedEvent>   maximizedEventDispatcher;
		EventDispatcher<Input::Events::WindowFocusGainedEvent> focusGainedEventDispatcher;
		EventDispatcher<Input::Events::WindowFocusLostEvent>   focusLostEventDispatcher;
		EventDispatcher<Input::Events::WindowCloseEvent>       closeEventDispatcher;
		EventDispatcher<Input::Events::WindowMouseEnterEvent>  mouseEnterDispatcher;
		EventDispatcher<Input::Events::WindowMouseLeaveEvent>  mouseLeaveDispatcher;
		
		Window(WindowHandle handle);
		Window(const Window&) = delete;
		Window(Window&&) noexcept = delete;
		~Window();

		Vec2i GetPos() const;
		Vec2i GetSize() const;
		String GetTitle() const;

		Result SetIcon(BitmapView bitmap);
		void SetOpacity(float opacity);
		void SetPos(Vec2i pos);
		void SetSize(Vec2i size);
		void SetTitle(StringView title);
		void SetMinimumSize(Vec2i size);
		void SetMaximumSize(Vec2i size);

		void Minimize();
		void Maximize();
		
		void Raise();

		void ShowWindow(bool show);				

		void SetWindowFullscreenMode(bool fullscreen);
		void SetWindowBorderFlag(bool hasBorder);
		void SetWindowResizableFlag(bool resizable);
		void SetLockMouseFlag(bool lockMouse);		

		bool IsFullscreen();
		bool IsBorderless();		
		bool IsResizable();
		bool IsMouseLocked();
		bool IsMinmized();
		bool IsMaximized();
		bool IsShown();

		uint GetWindowVideoDisplayIndex();
		DisplayMode GetWindowDisplayMode();
		void SetWindowDisplayMode(DisplayMode mode);
		
		inline WindowHandle GetHandle() const { return windowHandle; }

		Window& operator=(const Window&) = delete;		
		Window& operator=(Window&&) noexcept = delete;		
		
	public:
		WindowHandle windowHandle;					
	};
}