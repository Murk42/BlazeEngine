#include "BlazeEngine/Graphics/UI/States/StateManager.h"
#include "BlazeEngine/Graphics/UI/States/State.h"

namespace Blaze::UI
{
	void StateManager::MousePressedEvent(InputEvent::MousePressed)
	{
		Deselect();		
	}
	StateManager::StateManager()
		: layer(nullptr), selected(nullptr)
	{
	}
	void StateManager::Select(State& state)
	{
		selected = &state;
		state.selected = true;
		if (selected->SelectedEvent)
			selected->SelectedEvent();
		if (selected->StateChangedEvent)
			selected->StateChangedEvent();
	}
	void StateManager::Deselect()
	{
		if (selected != nullptr)
		{
			State* old = nullptr;
			bool was = false;
			if (selected != nullptr)
			{
				old = selected;
				was = old->selected;
				selected->selected = false;
				selected = nullptr;
			}
			if (old != nullptr && was)
			{
				if (old->DeselectedEvent)
					old->DeselectedEvent();
				if (old->StateChangedEvent)
					old->StateChangedEvent();
			}
		}
	}
	void StateManager::AddState(State& state)
	{
		state.manager = this;
		states.emplace_back(&state);
	}
}