#pragma once
#include "BlazeEngine/Application/UI/UIComponent.h"
#include "BlazeEngine/Event/EventDispatcher.h"
#include "BlazeEngine/Event/EventHandler.h"
#include "BlazeEngine/Event/LambdaEventHandler.h"

namespace Blaze::UI::Components
{
	class BLAZE_API MouseTriggerSystem : public ECS::System
	{
	public:		
		void Update(ECS::Component*) override;

	};

	class BLAZE_API MouseTrigger : public UIComponent
	{
	public:
		struct MousePressed		{ MouseTrigger& trigger; Vec2i pos; double time; uint combo; };
		struct MouseReleased	{ MouseTrigger& trigger; Vec2i pos; double time; };
		struct MouseMotion		{ MouseTrigger& trigger; Vec2i pos; Vec2i delta; };
		struct MouseScroll		{ MouseTrigger& trigger; int value; };
		struct MouseEnter		{ MouseTrigger& trigger; };
		struct MouseLeave		{ MouseTrigger& trigger; };

		EventDispatcher<MousePressed > mousePressedDispatcher;
		EventDispatcher<MouseReleased> mouseReleasedDispatcher;
		EventDispatcher<MouseMotion  > mouseMotionDispatcher;
		EventDispatcher<MouseScroll  > mouseScrollDispatcher;
		EventDispatcher<MouseEnter   > mouseEnterDispatcher;
		EventDispatcher<MouseLeave   > mouseLeaveDispatcher;

		MouseTrigger();
		MouseTrigger(const MouseTrigger&) = delete;		

		inline bool IsMouseInside() const { return mouseInside; }
		
		COMPONENT(MouseTrigger, MouseTriggerSystem);

		friend class System;
	private:		
		bool mouseInside;		
	};		
};