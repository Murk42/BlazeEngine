#pragma once

namespace Blaze::UI
{
	class Node;
	class InputManager;

	class BLAZE_GRAPHICS_API UIKeyboardEventHandler
	{
	public:
		struct KeyDownEvent   
		{ 
			InputManager& inputManager;
			Node& node;
			uint64 timeNS;
			Keyboard::KeyboardID keyboardID;

			Keyboard::Key key;
			Keyboard::KeyModifier modifier;
			Keyboard::KeyCode keyCode;

			bool repeat;			
		};
		struct KeyUpEvent     
		{ 
			InputManager& inputManager; 			
			Node& node;
			uint64 timeNS;
			Keyboard::KeyboardID keyboardID;

			Keyboard::Key key;
			Keyboard::KeyModifier modifier;
			Keyboard::KeyCode keyCode;
		};
		struct TextInputEvent 
		{ 
			InputManager& inputManager; 
			Node& node;
			uint64 timeNS;

			StringUTF8 string; 
		};

		virtual void OnEvent(const KeyDownEvent& event) { }
		virtual void OnEvent(const KeyUpEvent& event) { }
		virtual void OnEvent(const TextInputEvent& event) { }
	};
}