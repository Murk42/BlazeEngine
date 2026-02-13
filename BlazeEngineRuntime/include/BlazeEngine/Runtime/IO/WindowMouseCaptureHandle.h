#pragma once
#include "BlazeEngine/Core/Type/Variant.h"
#include "BlazeEngine/Runtime/IO/InputEvents.h"
#include "BlazeEngine/Core/Event/EventHandlerData.h"

namespace Blaze
{
	class WindowMouseCaptureHandle
	{
	public:
		struct MouseCaptureLostEvent { WindowMouseCaptureHandle& handle; };

		using CapturedMouseEvent = Variant<Input::MouseButtonDownEvent, Input::MouseButtonUpEvent, Input::MouseScrollEvent, MouseCaptureLostEvent>;

		WindowMouseCaptureHandle();
		WindowMouseCaptureHandle(EventHandlerData<CapturedMouseEvent> eventHandlerData);
		~WindowMouseCaptureHandle();

		bool IsCapturingMouse();
		bool ReleaseMouseCapture();

		void SetEventHandlerData(EventHandlerData<CapturedMouseEvent> eventHandlerData);

		WindowMouseCaptureHandle& operator=(WindowMouseCaptureHandle&& other) noexcept = delete;
	private:
		Window* window;
		Input::MouseID mouseID;
		EventHandlerData<CapturedMouseEvent> eventHandlerData;

		friend class Window;
	};
}