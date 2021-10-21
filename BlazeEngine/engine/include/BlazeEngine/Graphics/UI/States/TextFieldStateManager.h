#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Input/Event.h"

#include <vector>

namespace Blaze::UI
{
	class StateManager;
	class TextFieldState;	

	class BLAZE_API TextFieldStateManager
	{				
		StateManager* stateManager;
		std::vector<TextFieldState*> textFields;
	public:						
		TextFieldStateManager();

		void AddTextField(TextFieldState&);

		void MouseMotionEvent(InputEvent::MouseMotion);
		void MousePressedEvent(InputEvent::MousePressed);
		void MouseReleasedEvent(InputEvent::MouseReleased);
		void KeyPressedEvent(InputEvent::KeyPressed);
		void TextInputEvent(InputEvent::TextInput);

		friend class Layer;
	};
}