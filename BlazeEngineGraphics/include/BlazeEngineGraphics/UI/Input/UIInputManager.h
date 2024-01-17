#pragma once
#include "BlazeEngine/Input/InputEvents.h"
#include "BlazeEngineGraphics/UI/Core/UIScreen.h"
#include "UIInputNode.h"

namespace Blaze::UI
{
	class UIInputManager :
		EventHandler<NodeCreatedEvent>,
		EventHandler<NodeDestroyedEvent>,
		EventHandler<ScreenDestructionEvent>,
		EventHandler<Input::Events::InputPreUpdateEvent>,
		EventHandler<Input::Events::InputPostUpdateEvent>,
		EventHandler<Input::Events::MouseMotion>,
		EventHandler<Input::Events::MouseScroll>,
		EventHandler<Input::Events::KeyPressed>,
		EventHandler<Input::Events::KeyReleased>,
		EventHandler<Input::Events::TextInput>
	{
	public:
		UIInputManager();
		~UIInputManager();

		void SetWindow(WindowBase* window);
		void SetScreen(Screen* screen);		

		void SelectNode(nullptr_t null);
		void SelectNode(Node* selectedNode);
		void SelectNode(UIInputNode* selectedNode);

		inline WindowBase* GetWindow() const { return window; }
		inline Screen* GetScreen() const { return screen; }
		inline UIInputNode* GetSelectedNode() const { return selectedNode; }
	private:
		WindowBase* window;
		Screen* screen;		
		Vec2f mousePos;		

		bool recreateInputNodes;		
		Array<UIInputNode*> nodes;
		UIInputNode* selectedNode;

		void RecreateInputNodes();

		void CheckMousePositionEvents(Vec2i delta);
		void CheckMouseMotionEvents(UIMouseEventHandler* mouseEventHandler, Vec2i delta);
		void CheckMouseHitEvents(UIMouseEventHandler* mouseEventHandler, bool oldHit, bool newHit);

		void OnEvent(NodeCreatedEvent event) override;
		void OnEvent(NodeDestroyedEvent event) override;
		void OnEvent(ScreenDestructionEvent event) override;
		void OnEvent(Input::Events::InputPreUpdateEvent event) override;
		void OnEvent(Input::Events::InputPostUpdateEvent event) override;
		void OnEvent(Input::Events::MouseMotion event) override;
		void OnEvent(Input::Events::MouseScroll event) override;
		void OnEvent(Input::Events::KeyPressed event) override;
		void OnEvent(Input::Events::KeyReleased event) override;
		void OnEvent(Input::Events::TextInput event) override;
	};
}