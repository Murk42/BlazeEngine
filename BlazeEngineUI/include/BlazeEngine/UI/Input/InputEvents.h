#pragma once
#include "BlazeEngine/Runtime/IO/InputEvents.h"

namespace Blaze::UI
{
	class InputSubSystem;

	struct UIMouseButtonDownEvent : Input::MouseButtonDownEvent 
	{ 
		InputSubSystem& inputSubSystem; 
	};
	struct UIMouseButtonUpEvent : Input::MouseButtonUpEvent
	{
		InputSubSystem& inputSubSystem;
	};
	struct UIMouseMotionEvent : Input::MouseMotionEvent 
	{ 
		InputSubSystem& inputSubSystem;
	};
	struct UIMouseHitStatusChangedEvent
	{
		InputSubSystem& inputSubSystem;
		Node::HitStatus oldHitStatus;
		Node::HitStatus newHitStatus;
	};
	struct UIMouseScrollEvent : Input::MouseScrollEvent
	{
		InputSubSystem& inputSubSystem;
	};
	struct UIKeyDownEvent : Input::KeyDownEvent
	{
		InputSubSystem& inputSubSystem;
	};
	struct UIKeyUpEvent : Input::KeyUpEvent
	{
		InputSubSystem& inputSubSystem;
	};
	struct UITextInputEvent : Input::TextInputEvent
	{
		InputSubSystem& inputSubSystem;
	};

	using GenericUIInputEvent = Variant<UIKeyDownEvent, UIKeyUpEvent, UITextInputEvent, UIMouseButtonDownEvent, UIMouseButtonUpEvent, UIMouseMotionEvent, UIMouseScrollEvent>;
}