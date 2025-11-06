#include "pch.h"
#include "BlazeEngine/Runtime/Input.h"
#include "BlazeEngine/UI/Common/ButtonBase.h"
#include "BlazeEngine/UI/Input/InputManager.h"

namespace Blaze::UI
{
	ButtonBase::ButtonBase()
		: pressed(false), pressable(true)
	{
		//SetMouseMotionReportPolicy(MouseMotionReportPolicy::ReportWhileSelected);

		mouseButtonDownEventDispatcher.AddHandler<&ButtonBase::MouseButtonDownEvent>(*this);
		mouseButtonUpEventDispatcher.AddHandler<&ButtonBase::MouseButtonUpEvent>(*this);
		selectedStateChangedEventDispatcher.AddHandler<&ButtonBase::SelectedStateChangedEvent>(*this);
	}
	ButtonBase::~ButtonBase()
	{
		mouseButtonDownEventDispatcher.RemoveHandler<&ButtonBase::MouseButtonDownEvent>(*this);
		mouseButtonUpEventDispatcher.RemoveHandler<&ButtonBase::MouseButtonUpEvent>(*this);
		selectedStateChangedEventDispatcher.RemoveHandler<&ButtonBase::SelectedStateChangedEvent>(*this);
	}
	int ButtonBase::HitTest(Vec2f screenPos)
	{
		return Node::HitTest(screenPos) > 0 ? 2 : 0;
	}
	void ButtonBase::SetPressedEventCallback(const std::function<void(const PressedEvent&)>& callback)
	{
		this->pressedEventCallback = callback;
	}
	void ButtonBase::SetPressableFlag(bool pressable)
	{
		if (this->pressable != pressable)
		{
			this->pressable = pressable;

			if (!pressable && pressed)
			{
				pressed = false;
				pressableFlagChangedEventDispatcher.Call({ *this });
			}
		}
	}
	void ButtonBase::Unpress(bool quiet)
	{
		if (pressed)
		{
			pressed = false;

			if (!quiet)
				if (pressedEventCallback)
					pressedEventCallback({ *this });

			pressedStateChangedEventDispatcher.Call({ *this });
		}
	}
	void ButtonBase::Press()
	{
		if (!pressable)
		{
			pressed = true;

			pressedStateChangedEventDispatcher.Call({ *this });
		}
	}
	void ButtonBase::MouseButtonDownEvent(const InputNode::MouseButtonDownEvent& event)
	{
		if (event.button != Input::MouseButton::Left || GetMouseHitStatus() != 2)
			return;

		if (pressable)
		{
			pressed = true;
			pressedStateChangedEventDispatcher.Call({ *this });
		}

		Select();
	}
	void ButtonBase::MouseButtonUpEvent(const InputNode::MouseButtonUpEvent& event)
	{
		if (event.button != Input::MouseButton::Left)
			return;

		if (pressed)
			if (pressedEventCallback)
				pressedEventCallback({ *this });

		if (pressed)
		{
			pressed = false;
			pressedStateChangedEventDispatcher.Call({ *this });
		}
	}
	void ButtonBase::SelectedStateChangedEvent(const InputNode::SelectedStateChangedEvent& event)
	{
		if (!IsSelected())
			Unpress(true);
	}
}