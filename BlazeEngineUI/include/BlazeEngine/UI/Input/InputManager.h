#pragma once
#include "BlazeEngine/Runtime/Input.h"
#include "BlazeEngine/UI/Core/Screen.h"
#include "BlazeEngine/UI/Input/InputNode.h"

namespace Blaze::UI
{
	class BLAZE_API InputManager
	{
	public:
		InputManager(Screen& screen);
		~InputManager();

		inline InputNode* GetSelectedNode() const { return selectedNode; }

		void SelectNode(InputNode* node);
		void UpdateNodeHitStatus(InputNode& node);

		inline Vec2f GetScreenMousePos() const { return screenMousePos; }

		static InputManager* GetScreenInputManager(Screen& screen);
		static InputManager& CreateScreenInputManager(Screen& screen);
	private:
		Screen& screen;
		InputNode* selectedNode;

		Node* blockingNode;
		Vec2f screenMousePos;
		Vec2f windowMousePos;

		bool castMousePointer;

		void SubscribeToWindow(Window& window);
		void UnsubscribeFromWindow(Window& window);

		//This should be called only when the screen has a window
		void UpdateScreenMousePos(Vec2f windowPos, Vec2u windowSize);

		void CastMousePointer(Vec2f delta, uint64 timeNS);

		void InputPostUpdateEvent(const Input::InputPostUpdateEvent& event);
		void ScreenWindowChangedEvent(const ScreenWindowChangedEvent& event);
		void WindowFocusLostEvent(const Window::WindowFocusLostEvent& event);
		void MouseMotionEvent(const Input::MouseMotionEvent& event);
		void MouseScrollEvent(const Input::MouseScrollEvent& event);
		void MouseButtonDownEvent(const Input::MouseButtonDownEvent& event);
		void MouseButtonUpEvent(const Input::MouseButtonUpEvent& event);
		void KeyDownEvent(const Input::KeyDownEvent& event);
		void KeyUpEvent(const Input::KeyUpEvent& event);
		void TextInputEvent(const Input::TextInputEvent& event);
	};
}