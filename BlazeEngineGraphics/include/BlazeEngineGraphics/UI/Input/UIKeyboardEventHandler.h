#pragma once

namespace Blaze::UI
{
	class InputManager;
	class BLAZE_GRAPHICS_API UIKeyboardEventHandler
	{
	public:
		struct KeyPressedEvent { InputManager* inputManager; Key key; TimePoint time; uint combo; };
		struct KeyReleasedEvent { InputManager* inputManager; Key key; TimePoint time; };
		struct TextInputEvent { InputManager* inputManager; StringUTF8 input; };

		virtual void OnEvent(KeyPressedEvent event) { }
		virtual void OnEvent(KeyReleasedEvent event) { }
		virtual void OnEvent(TextInputEvent event) { }
	};
}