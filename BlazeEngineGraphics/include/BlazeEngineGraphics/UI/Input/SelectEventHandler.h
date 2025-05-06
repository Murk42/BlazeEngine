#pragma once

namespace Blaze::UI
{	
	class InputManager;
	class Node;

	class BLAZE_GRAPHICS_API UISelectEventHandler
	{
	public:
		struct SelectedEvent 
		{
			InputManager& inputManager; 
			Node& node;
		};
		struct DeselectedEvent 
		{ 
			InputManager& inputManager; 
			Node& node;
		};	

		UISelectEventHandler();

		void SetSelectableFlag(bool selectable);
		inline bool IsSelectable() const { return selectable; }
		
		virtual void OnEvent(const SelectedEvent& event) { }
		virtual void OnEvent(const DeselectedEvent& event) { }
	private:
		bool selectable = true;
	};
}