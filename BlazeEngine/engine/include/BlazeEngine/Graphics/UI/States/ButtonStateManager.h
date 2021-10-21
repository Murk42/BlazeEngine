#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Input/Event.h"
#include <vector>

namespace Blaze
{
	class Engine;
}

namespace Blaze::UI
{
	class ButtonState;

	class ButtonStateManager
	{
		std::vector<ButtonState*> buttons;

		void MouseMotionEvent(InputEvent::MouseMotion);
		void MousePressedEvent(InputEvent::MousePressed);
		void MouseReleasedEvent(InputEvent::MouseReleased);
	public:
		void AddButton(ButtonState& button);

		friend class LayerManager;
		friend class ::Blaze::Engine;
	};
}