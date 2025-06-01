#pragma once
#include "BlazeEngine/Input/Keyboard.h"
#include "BlazeEngine/Input/Mouse.h"
#include "BlazeEngineGraphics/UI/Core/Screen.h"
#include "BlazeEngineGraphics/UI/Input/InputNode.h"

namespace Blaze::UI
{
	class InputManager		
	{
	public:				
		InputManager(Screen& screen);
		~InputManager();

		inline InputNode* GetSelectedNode() const { return selectedNode; }

		void SelectNode(InputNode* node);		
		void UpdateNodeHitStatus(InputNode& node);

		static InputManager* GetScreenInputManager(Screen& screen);
		static InputManager& CreateScreenInputManager(Screen& screen);
	private:				
		Screen& screen;
		InputNode* selectedNode;

		Node* blockingNode;
		Vec2f screenMousePos;		
		Vec2f windowMousePos;
		Vec2f desktopMousePos;
		
		void SubscribeToWindow(Window& window);						
		void UnsubscribeFromWindow(Window& window);

		//This should be called only when the screen has a window
		void UpdateScreenMousePos(Vec2f windowPos);
		
		void CastMousePointer(Vec2f delta, uint64 timeNS);		
						
		void ScreenWindowChangedEvent(const ScreenWindowChangedEvent& event);
		void WindowFocusLostEvent(const Window::WindowFocusLostEvent& event);
		void MouseMotionEvent(const Mouse::MouseMotionEvent& event);
		void MouseScrollEvent(const Mouse::MouseScrollEvent& event);
		void MouseButtonDownEvent(const Mouse::MouseButtonDownEvent& event);
		void MouseButtonUpEvent(const Mouse::MouseButtonUpEvent& event);
		void KeyDownEvent(const Keyboard::KeyDownEvent& event);
		void KeyUpEvent(const Keyboard::KeyUpEvent& event);
		void TextInputEvent(const Keyboard::TextInputEvent& event);
	};
}