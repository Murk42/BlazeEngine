#include "pch.h"
#include "BlazeEngine/Application/UI/Components/MouseTriggerComponent.h"
#include "BlazeEngine/Application/UI/Components/UITransformComponent.h"
#include "BlazeEngine/Input/Input.h"
#include "BlazeEngine/Event/LambdaEventHandler.h"

namespace Blaze::UI::Components
{
	void MouseTriggerSystem::Update(ECS::Component* component)
	{
		MouseTrigger& trigger = *(MouseTrigger*)component;
		auto transform = trigger.transform;

		Vec2i mp = Input::GetMousePos();

		Vec2f pos = transform->GetFinalPos();
		Vec2f size = transform->GetFinalSize();

		bool wasInside = trigger.mouseInside;

		bool isInside = 
			mp.x >= pos.x && mp.x <= pos.x + size.x &&
			mp.y >= pos.y && mp.y <= pos.y + size.y;	

		if (!wasInside && isInside)		
			trigger.mouseEnterDispatcher.Call({ trigger });
		
		if (isInside)
		{					
			int scroll = Input::GetMouseScroll();
			Vec2i movement = Input::GetMouseMovement();		
			Vec2i pos = Input::GetMousePos();
			auto state = Input::GetKeyState(Key::MouseLeft);			

			if (state.pressed)
				trigger.mousePressedDispatcher.Call({ trigger, pos, state.time, state.combo });

			if (scroll > 0)
				trigger.mouseScrollDispatcher.Call({ trigger, scroll });

			if (movement != Vec2i())
				trigger.mouseMotionDispatcher.Call({ trigger, pos, movement });

			if (state.released)
				trigger.mouseReleasedDispatcher.Call({ trigger, pos, state.time });				
		} 
		
		if (wasInside && !isInside)
			trigger.mouseLeaveDispatcher.Call({ trigger });
	}	

	MouseTrigger::MouseTrigger()
		: mouseInside(false) 
	{		
	}		
}
