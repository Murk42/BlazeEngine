#pragma once
#include "BlazeEngine/Input/InputEvents.h"

namespace Blaze::UI
{	
	class UIInputManager;

	class UISelectEventHandler
	{
	public:
		struct SelectedEvent { UIInputManager* inputManager; };
		struct DeselectedEvent { UIInputManager* inputManager; };		

		bool selectable = true;

		virtual void OnEvent(Input::Events::KeyPressed event) { }
		virtual void OnEvent(Input::Events::KeyReleased event) { }
		virtual void OnEvent(Input::Events::TextInput event) { }
		virtual void OnEvent(SelectedEvent event) { }
		virtual void OnEvent(DeselectedEvent event) { }
	};
}