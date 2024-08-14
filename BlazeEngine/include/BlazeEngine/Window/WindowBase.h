#pragma once
#include "BlazeEngine/Resources/Bitmap/Bitmap.h"
#include "BlazeEngine/Event/EventDispatcher.h"
#include "BlazeEngine/Input/InputEvents.h"

namespace Blaze
{
	class BlazeEngineContext;

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
	
	class BLAZE_API WindowBase
	{
	public:		
		struct KeyState
		{
			bool pressed : 1;
			bool down : 1;
			bool released : 1;
			bool up : 1;
			uint8_t combo : 4;
			//Last moment the key state was updated, the value is got with TimePoint::GetRunTime()
			TimePoint lastPressedTime;
		};

		EventDispatcher<Input::Events::WindowResizedEvent>     resizedEventDispatcher;
		EventDispatcher<Input::Events::WindowMovedEvent>       movedEventDispatcher;
		EventDispatcher<Input::Events::WindowMinimizedEvent>   minimizedEventDispatcher;
		EventDispatcher<Input::Events::WindowMaximizedEvent>   maximizedEventDispatcher;
		EventDispatcher<Input::Events::WindowFocusGainedEvent> focusGainedEventDispatcher;
		EventDispatcher<Input::Events::WindowFocusLostEvent>   focusLostEventDispatcher;
		EventDispatcher<Input::Events::WindowCloseEvent>       closeEventDispatcher;
		EventDispatcher<Input::Events::WindowMouseEnterEvent>  mouseEnterDispatcher;
		EventDispatcher<Input::Events::WindowMouseLeaveEvent>  mouseLeaveDispatcher;
		EventDispatcher<Input::Events::KeyPressed>             keyPressedDispatcher;
		EventDispatcher<Input::Events::KeyReleased>            keyReleasedDispatcher;
		EventDispatcher<Input::Events::MouseMotion>            mouseMotionDispatcher;
		EventDispatcher<Input::Events::MouseScroll>            mouseScrollDispatcher;
		EventDispatcher<Input::Events::TextInput>              textInputDispatcher;						
		
		WindowBase();
		WindowBase(const WindowBase&) = delete;
		WindowBase(WindowBase&&) noexcept;		
		virtual ~WindowBase();

		virtual Vec2i GetPos() const = 0;
		virtual Vec2u GetSize() const = 0;
		virtual StringUTF8 GetTitle() const = 0;
		virtual Vec2i GetMousePos() const = 0;

		virtual Result SetIcon(BitmapView bitmap) = 0;
		virtual void SetOpacity(float opacity) = 0;
		virtual void SetPos(Vec2i pos) = 0;
		virtual void SetSize(Vec2u size) = 0;
		virtual void SetTitle(StringViewUTF8 title) = 0;
		virtual void SetMinimumSize(Vec2u size) = 0;
		virtual void SetMaximumSize(Vec2u size) = 0;

		virtual void Minimize() = 0;
		virtual void Maximize() = 0;

		virtual void Raise() = 0;

		virtual void ShowWindow(bool show) = 0;

		virtual void SetWindowFullscreenMode(bool fullscreen) = 0;
		virtual void SetWindowBorderFlag(bool hasBorder) = 0;
		virtual void SetWindowResizableFlag(bool resizable) = 0;
		virtual void SetWindowLockMouseFlag(bool lockMouse) = 0;

		virtual bool IsFullscreen() const = 0;
		virtual bool IsBorderless() const = 0;
		virtual bool IsResizable() const = 0;
		virtual bool IsMouseLocked() const = 0;
		virtual bool IsMinimized() const = 0;
		virtual bool IsMaximized() const = 0;
		virtual bool IsShown() const = 0;

		virtual uint GetWindowVideoDisplayIndex() const = 0;
		virtual DisplayMode GetWindowDisplayMode() const = 0;
		virtual void SetWindowDisplayMode(DisplayMode mode) = 0;

		KeyState GetLastKeyState(Key key) const;
		void ResetPressedAndReleasedKeys();		

		WindowBase& operator=(const WindowBase&) = delete;
		WindowBase& operator=(WindowBase&&) noexcept;
	protected:		
		std::mutex mutex;

		WindowBase(WindowBase&&, const std::lock_guard<std::mutex>& lk) noexcept;

		virtual void HandleResizeEvent(const Input::Events::WindowResizedEvent& event) { }
	private:						

		//This doesn't limit how much key presses can be detected at once, only the
		//size of the array which is used to speed-up these events
		static constexpr uintMem MAX_PRESSED_KEY_COUNT = 8;
		static constexpr uintMem MAX_RELEASED_KEY_COUNT = 8;

		using Allocator = DefaultAllocator;
		Allocator allocator;		
				
		KeyState keyStates[(uint)Key::KeyCount];	
		Key pressedKeys[MAX_PRESSED_KEY_COUNT]{ };
		fitted_uint<MAX_PRESSED_KEY_COUNT> pressedKeyCount;
		Key releasedKeys[MAX_PRESSED_KEY_COUNT]{ };
		fitted_uint<MAX_RELEASED_KEY_COUNT> releasedKeyCount;				


		void HandleKeyEvent(Input::Events::KeyPressed event);
		void HandleKeyEvent(Input::Events::KeyReleased event);

		friend class BlazeEngineContext;
	};		
}