#pragma once
#include "BlazeEngine/Core/EngineCore.h"

#include "BlazeEngine/Graphics/UI/States/State.h"

namespace Blaze::UI
{
	class Element;
	class BLAZE_API ButtonState : public State
	{
	public:
		Element* parent;
		bool hovered;
		bool down;
		std::function<void()> StateChangedEvent;
		std::function<void()> PressedEvent;
		std::function<void()> ReleasedEvent;		

		ButtonState();

		friend class ButtonStateManager;
	};
}