#pragma once
#include "BlazeEngine/Input/InputEvents.h"
#include "BlazeEngineGraphics/UI/Core/UIScreen.h"
#include "UIInputNode.h"

namespace Blaze::UI
{
	class InputManager :		
		EventHandler<NodeTreeChangedEvent>,
		EventHandler<ScreenDestructionEvent>,
		EventHandler<ScreenWindowChangedEvent>,				
		EventHandler<Input::Events::MouseMotion>,
		EventHandler<Input::Events::MouseScroll>,
		EventHandler<Input::Events::KeyPressed>,
		EventHandler<Input::Events::KeyReleased>,
		EventHandler<Input::Events::TextInput>
	{
	public:
		InputManager();
		~InputManager();		

		void SetScreen(Screen* screen);
		
		void SelectNode(InputNode* selectedNode);
		
		inline InputNode* GetSelectedNode() const { return selectedNode; }
	private:
		class InputNodeFinalTransformUpdatedEventHandler : public EventHandler<Node::FinalTransformUpdatedEvent>
		{
		public:
			InputNodeFinalTransformUpdatedEventHandler(InputManager* inputManager);

			void OnEvent(Node::FinalTransformUpdatedEvent event) override;

		private:
			InputManager* inputManager;
		};
		class InputNodeEnabledStateUpdatedEventHandler : public EventHandler<Node::EnabledStateUpdatedEvent>
		{
		public:
			InputNodeEnabledStateUpdatedEventHandler(InputManager* inputManager);

			void OnEvent(Node::EnabledStateUpdatedEvent event) override;

		private:
			InputManager* inputManager;
		};

		struct InputNodeData
		{
			bool hit;
			InputNode& node;
			InputNodeFinalTransformUpdatedEventHandler finalTransformUpdatedEventHandler;
			InputNodeEnabledStateUpdatedEventHandler enabledStateUpdatedEventHandler;
		};
		Array<InputNodeData> nodesData;

		InputNode* selectedNode;
		Screen* screen;

		uintMem mouseBlockInputNodeIndex;
		Vec2f screenMousePos;

		bool recreateScreenNodes;

		Array<Node*> castMousePointerBFSBuffer;
		
		void SubscribeToScreen(Screen& screen);						
		void UnsubscribeFromScreen(Screen& screen);
		void SubscribeToWindow(WindowBase& window);						
		void UnsubscribeFromWindow(WindowBase& window);

		void UpdateScreenMousePos();

		void GatherInputNodes(Node& node);
		//Expects that screen is not nullptr
		void RecreateScreenInputNodes();
		//Updates all nodes mouse interaction beginning from 'beginIndex'. beginIndex is expected to be less than mouseBlockInputNodeIndex
		void CastMousePointer(uintMem beginIndex, Vec2f mouseDelta);
		void CastMousePointer(Node* startNode, Vec2f mouseDelta);
		//This function will be called by the nodes event handler if it is enabled.
		void NodeFinalTransformUpdated(InputNode& node);
		//This function will be called by the nodes event handler if the node was disabled
		void NodeDisabled(InputNode& node);		

		
		void OnEvent(NodeTreeChangedEvent event) override;
		void OnEvent(ScreenDestructionEvent event) override;
		void OnEvent(ScreenWindowChangedEvent event) override;	
		void OnEvent(Input::Events::MouseMotion event) override;
		void OnEvent(Input::Events::MouseScroll event) override;
		void OnEvent(Input::Events::KeyPressed event) override;
		void OnEvent(Input::Events::KeyReleased event) override;
		void OnEvent(Input::Events::TextInput event) override;
	};
}