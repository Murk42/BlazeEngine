#include "pch.h"
#include "BlazeEngineGraphics/UI/Common/ButtonBase.h"
#include "BlazeEngineGraphics/UI/Input/InputManager.h"

namespace Blaze::UI
{
	ButtonBase::ButtonBase()
		: pressed(false)
	{				
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
	void ButtonBase::OnEvent(const MouseButtonDownEvent& event)
	{
		if (pressable)
		{
			pressed = true;
			event.inputManager.SelectNode(this);

			pressedEventCallback({ event.inputManager, *this });
		}
	}
	void ButtonBase::OnEvent(const MouseButtonUpEvent& event)
	{
		pressed = false;
	}
	void ButtonBase::OnEvent(const DeselectedEvent& event)
	{
		pressed = false;
	}
}