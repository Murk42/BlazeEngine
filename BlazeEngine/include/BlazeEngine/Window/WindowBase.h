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
		EventDispatcher<Input::Events::KeyPressed            > keyPressedDispatcher;
		EventDispatcher<Input::Events::KeyReleased           > keyReleasedDispatcher;
		EventDispatcher<Input::Events::MouseMotion           > mouseMotionDispatcher;
		EventDispatcher<Input::Events::MouseScroll           > mouseScrollDispatcher;
		EventDispatcher<Input::Events::TextInput             > textInputDispatcher;
		
		WindowBase();
		WindowBase(const WindowBase&) = delete;
		WindowBase(WindowBase&&) noexcept;		
		virtual ~WindowBase();

		virtual Vec2i GetPos() const;
		virtual Vec2u GetSize() const;
		virtual StringUTF8 GetTitle() const;
		virtual Vec2i GetMousePos() const;

		virtual Result SetIcon(BitmapView bitmap);
		virtual void SetOpacity(float opacity);
		virtual void SetPos(Vec2i pos);
		virtual void SetSize(Vec2u size);
		virtual void SetTitle(StringViewUTF8 title);
		virtual void SetMinimumSize(Vec2u size);
		virtual void SetMaximumSize(Vec2u size);

		virtual void Minimize();
		virtual void Maximize();

		virtual void Raise();

		virtual void ShowWindow(bool show);

		virtual void SetWindowFullscreenMode(bool fullscreen);
		virtual void SetWindowBorderFlag(bool hasBorder);
		virtual void SetWindowResizableFlag(bool resizable);
		virtual void SetWindowLockMouseFlag(bool lockMouse);

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
		Allocator allocator;

		WindowBase* implementation;
	};	
}