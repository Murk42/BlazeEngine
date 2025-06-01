#include "pch.h"
#include "BlazeEngine/Input/Input.h"
#include "BlazeEngineGraphics/UI/Common/ButtonBase.h"
#include "BlazeEngineGraphics/UI/Input/InputManager.h"

namespace Blaze::UI
{
	ButtonBase::ButtonBase()
		: pressed(false), pressable(true)
	{		
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
	void ButtonBase::SetPressableFlag(bool pressable)
	{
		if (this->pressable != pressable)
		{
			this->pressable = pressable;

			if (!pressable && pressed)
				pressed = false;

			pressableFlagChangedEventDispatcher.Call({ *this });
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
		}
	}
	void ButtonBase::Press()
	{
		if (pressedEventCallback)
			pressedEventCallback({ *this });
	}
	void ButtonBase::MouseButtonDownEvent(const InputNode::MouseButtonDownEvent& event)
	{
		if (event.button != Mouse::MouseButton::Left)
			return;

		if (pressable)
			pressed = true;
		
		Select();		
	}
	void ButtonBase::MouseButtonUpEvent(const InputNode::MouseButtonUpEvent& event)
	{
		if (pressed && GetMouseHitStatus() != 0)
			if (pressedEventCallback)
				pressedEventCallback({ *this });
		pressed = false;
	}	
	void ButtonBase::SelectedStateChangedEvent(const InputNode::SelectedStateChangedEvent& event)
	{
		if (!IsSelected())
			Unpress(true);
	}
}