#pragma once
#include "BlazeEngine/Resources/Bitmap/Bitmap.h"
#include "BlazeEngine/Event/EventDispatcher.h"
#include "BlazeEngine/Input/InputEvents.h"
#include "BlazeEngine/Memory/VirtualAllocator.h"

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

	/*
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
	*/

	static constexpr bool a = IsStaticAllocator<VirtualAllocator>;
	static constexpr bool b = IsDynamicAllocator<VirtualAllocator>;
	static constexpr bool c = IsAllocator<VirtualAllocator>;
	
	class BLAZE_API WindowBase 
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
		
		WindowBase();
		WindowBase(const WindowBase&) = delete;
		WindowBase(WindowBase&&) noexcept;
		template<typename T>
		WindowBase(T&& implementation);
		virtual ~WindowBase();

		virtual Vec2i GetPos() const;
		virtual Vec2i GetSize() const;
		virtual String GetTitle() const;

		virtual Result SetIcon(BitmapView bitmap);
		virtual void SetOpacity(float opacity);
		virtual void SetPos(Vec2i pos);
		virtual void SetSize(Vec2i size);
		virtual void SetTitle(StringView title);
		virtual void SetMinimumSize(Vec2i size);
		virtual void SetMaximumSize(Vec2i size);

		virtual void Minimize();
		virtual void Maximize();

		virtual void Raise();

		virtual void ShowWindow(bool show);

		virtual void SetWindowFullscreenMode(bool fullscreen);
		virtual void SetWindowBorderFlag(bool hasBorder);
		virtual void SetWindowResizableFlag(bool resizable);
		virtual void SetLockMouseFlag(bool lockMouse);

		virtual bool IsFullscreen();
		virtual bool IsBorderless();
		virtual bool IsResizable();
		virtual bool IsMouseLocked();
		virtual bool IsMinmized();
		virtual bool IsMaximized();
		virtual bool IsShown();

		virtual uint GetWindowVideoDisplayIndex();
		virtual DisplayMode GetWindowDisplayMode();
		virtual void SetWindowDisplayMode(DisplayMode mode);

		inline WindowBase* GetImplementation() const { return implementation; }

		WindowBase& operator=(const WindowBase&) = delete;
		WindowBase& operator=(WindowBase&&) noexcept;
	private:
		using Allocator = DefaultAllocator;

		WindowBase* implementation;
	};

	template<typename T>
	WindowBase::WindowBase(T&& implementation)
		: implementation(Allocator::Create(std::move(implementation)))
	{
	}
}