#pragma once

namespace Blaze::UI
{
	class UIInputManager;

	class UIMouseEventHandler
	{
	public:
		enum class MouseButton {
			Left, Middle, Right,
		};

		struct MousePressedEvent { UIInputManager* inputManager; Vec2f pos; MouseButton button; double time; uint combo; };
		struct MouseReleasedEvent { UIInputManager* inputManager; Vec2f pos; MouseButton button; double time; };
		struct MouseEnterEvent { UIInputManager* inputManager; Vec2f pos; };
		struct MouseExitEvent { UIInputManager* inputManager; Vec2f pos; };
		struct MouseMotionEvent { UIInputManager* inputManager; Vec2f pos; Vec2f delta; };
		struct MouseScrollEvent { UIInputManager* inputManager; Vec2f pos; int value; };

		virtual void OnEvent(MouseMotionEvent event) {  }
		virtual void OnEvent(MouseScrollEvent event) {  }
		virtual void OnEvent(MousePressedEvent event) {  }
		virtual void OnEvent(MouseReleasedEvent event) {  }
		virtual void OnEvent(MouseEnterEvent event) { }
		virtual void OnEvent(MouseExitEvent event) { }	
	private:		
		uint32 lastTransformState;
	
		friend class UIInputManager;
	};
}