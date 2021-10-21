#include "BlazeEngine/Graphics/UI/States/ButtonState.h"

namespace Blaze::UI
{
	ButtonState::ButtonState()
		: hovered(false), down(false)
	{		
		State::StateChangedEvent = [&]()
		{
			if (ButtonState::StateChangedEvent)
				ButtonState::StateChangedEvent();
		};
	}
}