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
	class State;
	class Layer; 

	class BLAZE_API StateManager
	{				
		void MousePressedEvent(InputEvent::MousePressed);	

		Layer* layer;
		State* selected;
	public:
		StateManager();
		std::vector<State*> states;

		void Select(State& state);
		void Deselect();
		State* GetSelected() const { return selected; }

		void AddState(State& state);
		
		friend class Layer;
		friend class Engine;
	};
}