#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Transform2D.h"

#include <functional>

namespace Blaze::UI
{
	class StateManager;

	class BLAZE_API State
	{		
		StateManager* manager;
		bool selected;	
	public:					
		std::function<void()> StateChangedEvent;
		std::function<void()> SelectedEvent;
		std::function<void()> DeselectedEvent;

		void Select();
		inline bool IsSelected() const { return selected; }
		inline StateManager* GetStateManager() const { return manager; }

		friend class StateManager;
	};
}