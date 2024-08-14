#pragma once

namespace Blaze::UI
{
	class UIInputManager;
	class UIKeyboardEventHandler
	{
	public:
		struct KeyPressedEvent { UIInputManager* inputManager; Key key; TimePoint time; uint combo; };
		struct KeyReleasedEvent { UIInputManager* inputManager; Key key; TimePoint time; };
		struct TextInputEvent { UIInputManager* inputManager; StringUTF8 input; };

		virtual void OnEvent(KeyPressedEvent event) { }
		virtual void OnEvent(KeyReleasedEvent event) { }
		virtual void OnEvent(TextInputEvent event) { }
	};
}