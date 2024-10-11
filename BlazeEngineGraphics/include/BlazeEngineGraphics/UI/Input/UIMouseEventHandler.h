#pragma once

namespace Blaze::UI
{
	class InputManager;

	class BLAZE_GRAPHICS_API UIMouseEventHandler
	{
	public:
		enum class MouseButton {
			Left, Middle, Right,
		};

		struct MousePressedEvent { InputManager* inputManager; Vec2f pos; MouseButton button; TimePoint time; uint combo; };
		struct MouseReleasedEvent { InputManager* inputManager; Vec2f pos; MouseButton button; TimePoint time; };
		struct MouseEnterEvent { InputManager* inputManager; Vec2f pos; };
		struct MouseExitEvent { InputManager* inputManager; Vec2f pos; };
		struct MouseMotionEvent { InputManager* inputManager; Vec2f pos; Vec2f delta; };
		struct MouseScrollEvent { InputManager* inputManager; Vec2f pos; int value; };

		virtual void OnEvent(MouseMotionEvent event) {  }
		virtual void OnEvent(MouseScrollEvent event) {  }
		virtual void OnEvent(MousePressedEvent event) {  }
		virtual void OnEvent(MouseReleasedEvent event) {  }
		virtual void OnEvent(MouseEnterEvent event) { }
		virtual void OnEvent(MouseExitEvent event) { }	
	private:		
		uint32 lastTransformState;
	
		friend class InputManager;
	};
}