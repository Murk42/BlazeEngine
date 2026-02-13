#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/Math/Vector.h"
#include "BlazeEngine/Core/String/String.h"
#include "BlazeEngine/Core/Container/Map.h"
#include "BlazeEngine/Core/Common/EventQueue.h"
#include "BlazeEngine/Core/Type/Variant.h"
#include "BlazeEngine/Runtime/IO/Display.h"
#include "BlazeEngine/Common/Bitmap.h"
#include "BlazeEngine/Runtime/IO/InputEvents.h"
#include "BlazeEngine/Core/Event/EventDispatcher.h"
#include <functional>

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

	class WindowMouseCaptureHandle;

	class BLAZE_API Window
	{
	public:
		struct ResizedEvent             { Window& window; uint64 timeNS; Vec2u size;                        };
		struct MovedEvent               { Window& window; uint64 timeNS; Vec2i pos;                         };
		struct MinimizedEvent           { Window& window; uint64 timeNS;                                    };
		struct MaximizedEvent           { Window& window; uint64 timeNS;                                    };
		struct FocusGainedEvent         { Window& window; uint64 timeNS;                                    };
		struct FocusLostEvent           { Window& window; uint64 timeNS;                                    };
		struct MouseCaptureChangedEvent { Window& window; Input::MouseID id; bool captured;                 };
		struct CloseEvent               { Window& window; uint64 timeNS;                                    };
		struct DisplayChangedEvent      { Window& window; uint64 timeNS; Display::DisplayID newDisplayID;   };
		struct ContentScaleChangedEvent { Window& window; float newContentScale;                            };
		struct DestructionEvent         { Window& window; uint64 timeNS;                                    };

		EventDispatcher<ResizedEvent>                resizedEventDispatcher;
		EventDispatcher<MovedEvent>                  movedEventDispatcher;
		EventDispatcher<MinimizedEvent>              minimizedEventDispatcher;
		EventDispatcher<MaximizedEvent>              maximizedEventDispatcher;
		EventDispatcher<FocusGainedEvent>            focusGainedEventDispatcher;
		EventDispatcher<FocusLostEvent>              focusLostEventDispatcher;
		EventDispatcher<MouseCaptureChangedEvent>    mouseCaptureChagnedEvent;
		EventDispatcher<CloseEvent>                  closeEventDispatcher;
		EventDispatcher<DisplayChangedEvent>         displayChangedEventDispatcher;
		EventDispatcher<ContentScaleChangedEvent>    contentScaleChangedEventDispatcher;
		EventDispatcher<DestructionEvent>            destructionEventDispatcher;

		using GenericWindowEvent = Variant<ResizedEvent, MovedEvent, MinimizedEvent, MaximizedEvent, FocusGainedEvent, FocusLostEvent, CloseEvent, DisplayChangedEvent, ContentScaleChangedEvent>;

		Window();
		Window(const Window&) = delete;
		Window(Window&&) noexcept;
		Window(const WindowCreateOptions& createOptions);
		Window(void* handle, WindowGraphicsAPI graphicsAPI);
		virtual ~Window();

		virtual void Destroy();

		//Window basic properties

		u8String GetTitle() const;
		void SetTitle(u8StringView title);
		Vec2i GetPos() const;
		void SetPos(Vec2i pos);
		Vec2u GetSize() const;
		void SetSize(Vec2u size);

		//Window behavior properties

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

		bool CaptureMouse(Input::MouseID mouseID, WindowMouseCaptureHandle& mouseCaptureHandle);
		bool ReleaseMouse(Input::MouseID mouseID);
		bool IsMouseCaptured(Input::MouseID mouseID) const;

		bool GetDefaultMousePos(Vec2f& mousePos) const;
		Recti GetMouseConfinementRectangle() const;
		void SetMouseConfinementRectangle(Recti rect);
		bool GetRelativeMouseModeFlag() const;
		void SetRelativeMouseModeFlag(bool enabledFlag);
		bool GetKeyboardGrabFlag() const;
		void SetKeyboardGrabFlag(bool grab);
		void WarpMouseInWindow(Vec2f newMousePos);

		Window* GetParentWindow() const;
		Vec2u GetWindowMonitorDimensions() const;
		void Raise();

		void SetDisplayModeForFullScreen(const Display::DisplayMode& displayMode);
		Display::DisplayID GetDisplayIndex();

		bool ProcessInputEvent(Input::GenericInputEvent& event);
		bool HasInputEvents() const;
		bool WaitForEventsToBeProcessed(float seconds) const;
		void AddInputEvent(const Input::GenericInputEvent& event);
		void AddWindowEvent(const GenericWindowEvent& event);

		void* ReleaseHandle();
		inline void* GetHandle() const { return handle; }

		Window& operator=(const Window&) = delete;
		Window& operator=(Window&&) noexcept;
	private:
		using InternalEvent = Variant<Input::GenericInputEvent, GenericWindowEvent>;

		struct MouseCaptureData
		{
			WindowMouseCaptureHandle& handle;
		};

		EventQueue<InternalEvent> eventQueue;
		WindowGraphicsAPI graphicsAPI;
		void* handle;

		Map<Input::MouseID, MouseCaptureData> mouseCaptureData;

		Vec2i realWindowPos;
		Vec2u realWindowSize;
		Vec2i apparentWindowPos;
		Vec2u apparentWindowSize;

		void Move(Window& other);
		void HandleWindowEvent(const GenericWindowEvent& event);
	};
}