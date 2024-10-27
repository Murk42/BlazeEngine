#pragma once
#include "BlazeEngine/Input/InputEvents.h"
#include "BlazeEngineGraphics/UI/Core/UIScreen.h"
#include "UIInputNode.h"

namespace Blaze::UI
{
	class InputManager :
		EventHandler<NodeCreatedEvent>,
		EventHandler<NodeDestroyedEvent>,
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

		void SelectNode(nullptr_t null);
		void SelectNode(Node* selectedNode);
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

		struct InputNodeData
		{
			InputNode& node;
			InputNodeFinalTransformUpdatedEventHandler finalTransformUpdatedEventHandler;
		};
		Array<InputNodeData> nodesData;
		InputNode* selectedNode;
		Screen* screen;
		uintMem mouseBlockInputNodeIndex;

		bool recreateScreenNodes;
		
		void SubscribeToScreen(Screen& screen);		
		void UnsubscribeFromScreen(Screen& screen);		
		void SubscribeToWindow(WindowBase& window);		
		void UnsubscribeFromWindow(WindowBase& window);

		void GatherInputNodes(Node& node);
		//Expects that screen is not nullptr
		void RecreateScreenInputNodes();
		//Updates all nodes mouse interaction beginning from 'beginIndex'. beginIndex is expected to be less than mouseBlockInputNodeIndex
		void CastMousePointer(uintMem beginIndex, Vec2f mousePos, Vec2f mouseDelta);
		void NodeFinalTransformUpdated(InputNode& node);
		
		void UpdateMouseHitEvents(InputNode& node, Vec2i mouseDelta);

		void OnEvent(NodeCreatedEvent event) override;
		void OnEvent(NodeDestroyedEvent event) override;
		void OnEvent(ScreenDestructionEvent event) override;
		void OnEvent(ScreenWindowChangedEvent event) override;	
		void OnEvent(Input::Events::MouseMotion event) override;
		void OnEvent(Input::Events::MouseScroll event) override;
		void OnEvent(Input::Events::KeyPressed event) override;
		void OnEvent(Input::Events::KeyReleased event) override;
		void OnEvent(Input::Events::TextInput event) override;
	};
}