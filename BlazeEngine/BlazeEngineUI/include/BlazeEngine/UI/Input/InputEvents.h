#pragma once
#include "BlazeEngine/Runtime/IO/InputEvents.h"

namespace Blaze::UI
{
	class InputNode;

	struct UIMouseButtonDownEvent : Input::MouseButtonDownEvent 
	{ 
		InputNode& node;
	};
	struct UIMouseButtonUpEvent : Input::MouseButtonUpEvent
	{
		InputNode& node;
	};
	struct UIMouseMotionEvent : Input::MouseMotionEvent 
	{ 
		InputNode& node;
	};
	struct UIMouseHitStatusChangedEvent
	{
		InputNode& node;
		Node::HitStatus oldHitStatus;
		Node::HitStatus newHitStatus;
	};
	struct UIMouseScrollEvent : Input::MouseScrollEvent
	{
		InputNode& node;
	};
	struct UIKeyDownEvent : Input::KeyDownEvent
	{
		InputNode& node;
	};
	struct UIKeyUpEvent : Input::KeyUpEvent
	{
		InputNode& node;
	};
	struct UITextInputEvent : Input::TextInputEvent
	{
		InputNode& node;
	};

	using GenericUIInputEvent = Variant<UIKeyDownEvent, UIKeyUpEvent, UITextInputEvent, UIMouseButtonDownEvent, UIMouseButtonUpEvent, UIMouseMotionEvent, UIMouseScrollEvent>;
}