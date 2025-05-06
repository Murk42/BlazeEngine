#pragma once
#include "BlazeEngineGraphics/UI/Core/Screen.h"
#include "BlazeEngineGraphics/UI/Input/InputNode.h"

namespace Blaze::UI
{
	class InputManager :		
		EventHandler<NodeTreeChangedEvent>,
		EventHandler<ScreenDestructionEvent>,
		EventHandler<ScreenWindowChangedEvent>,						
		EventHandler<Mouse::MouseScrollEvent>,
		EventHandler<Mouse::MouseButtonDownEvent>,
		EventHandler<Mouse::MouseButtonUpEvent>,
		EventHandler<Keyboard::KeyDownEvent>,
		EventHandler<Keyboard::KeyUpEvent>,
		EventHandler<Keyboard::TextInputEvent>
	{
	public:
		InputManager();
		~InputManager();		

		void SetScreen(Screen* screen);
		
		void SelectNode(InputNode* selectedNode);
		
		inline InputNode* GetSelectedNode() const { return selectedNode; }
	private:				
		InputNode* selectedNode;
		Screen* screen;

		Node* blockingNode;		
		Vec2f screenMousePos;		
		Vec2f windowMousePos;
		Vec2f desktopMousePos;
		
		void SubscribeToScreen(Screen& screen);						
		void UnsubscribeFromScreen(Screen& screen);
		void SubscribeToWindow(Window& window);						
		void UnsubscribeFromWindow(Window& window);

		//This should be called only when the screen has a window
		void UpdateScreenMousePos(Vec2f windowPos);
		
		void CastMousePointer(Vec2f delta, uint64 timeNS);
		//This function will be called by the nodes event handler if it is enabled.
		void NodeFinalTransformUpdated(InputNode& node);
		//This function will be called by the nodes event handler if the node enabled state was changed
		void NodeEnabledStateChanged(InputNode& node);
		
		void OnEvent(const NodeTreeChangedEvent& event) override;
		void OnEvent(const ScreenDestructionEvent& event) override;
		void OnEvent(const ScreenWindowChangedEvent& event) override;	
		void Event_WindowFocusLost(const Window::WindowFocusLostEvent& event);
		void Event_MouseMotion(const Mouse::MouseMotionEvent& event);
		void OnEvent(const Mouse::MouseScrollEvent& event) override;
		void OnEvent(const Mouse::MouseButtonDownEvent& event) override;
		void OnEvent(const Mouse::MouseButtonUpEvent& event) override;
		void OnEvent(const Keyboard::KeyDownEvent& event) override;
		void OnEvent(const Keyboard::KeyUpEvent& event) override;
		void OnEvent(const Keyboard::TextInputEvent& event) override;

		static void SetInputManagerRecursively(Node* node, InputManager* ptr);

		friend class InputNode;
	};
}