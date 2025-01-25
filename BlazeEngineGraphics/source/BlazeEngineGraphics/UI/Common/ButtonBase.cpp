#include "pch.h"
#include "BlazeEngineGraphics/UI/Common/ButtonBase.h"
#include "BlazeEngineGraphics/UI/Input/UIInputManager.h"

namespace Blaze::UI
{
	ButtonBase::ButtonBase()
		: pressed(false)
	{		
		SetBlocksMouseEventsFlag(true);
	}
	void ButtonBase::OnEvent(MousePressedEvent event)
	{
		pressed = true;
		event.inputManager->SelectNode(this);		

		pressedEventDispatcher.Call({ event.inputManager });
	}
	void ButtonBase::OnEvent(MouseReleasedEvent event)
	{
		pressed = false;
	}
	void ButtonBase::OnEvent(DeselectedEvent event)
	{
		pressed = false;
	}
}