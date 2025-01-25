#pragma once
#include "BlazeEngine/Input/InputEvents.h"

namespace Blaze::UI
{	
	class InputManager;

	class BLAZE_GRAPHICS_API UISelectEventHandler
	{
	public:
		struct SelectedEvent { InputManager* inputManager; };
		struct DeselectedEvent { InputManager* inputManager; };	

		UISelectEventHandler();

		void SetSelectableFlag(bool selectable);
		inline bool IsSelectable() const { return selectable; }

		virtual void OnEvent(Input::Events::KeyPressed event) { }
		virtual void OnEvent(Input::Events::KeyReleased event) { }
		virtual void OnEvent(Input::Events::TextInput event) { }
		virtual void OnEvent(SelectedEvent event) { }
		virtual void OnEvent(DeselectedEvent event) { }
	private:
		bool selectable = true;
	};
}