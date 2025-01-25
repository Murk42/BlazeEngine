#include "pch.h"
#include "BlazeEngineGraphics/UI/Input/UIInputManager.h"
#include "BlazeEngineGraphics/UI/Input/UIMouseEventHandler.h"
#include "BlazeEngineGraphics/UI/Input/UIKeyboardEventHandler.h"
#include "BlazeEngineGraphics/UI/Input/UISelectEventHandler.h"
#include "BlazeEngine/Input/Input.h"

namespace Blaze::UI
{
	//TODO update mouse cast when InputNode::blocksMouseEvents is changed 
	//TODO update node ordering when parent is changed

	InputManager::InputManager()
		: selectedNode(nullptr), screen(nullptr), mouseBlockInputNodeIndex(0), recreateScreenNodes(false)
	{				
	}
	InputManager::~InputManager()
	{
		if (screen != nullptr)
			UnsubscribeFromScreen(*screen);
	}
	void InputManager::SetScreen(Screen* screen)
	{
		if (this->screen != nullptr)
			UnsubscribeFromScreen(*this->screen);

		this->screen = screen;

		if (this->screen != nullptr)
		{
			SubscribeToScreen(*this->screen);
			recreateScreenNodes = true;
		}
		else
		{
			recreateScreenNodes = false;
		}
	}
	void InputManager::SelectNode(InputNode* newSelectedInputNode)
	{
		if (selectedNode == newSelectedInputNode)
			return;

		if (newSelectedInputNode != nullptr && !newSelectedInputNode->IsEnabled())
		{
			Debug::Logger::LogError("Blaze Engine Graphics", "Trying to select a disabled node");
			return;
		}

		if (newSelectedInputNode != nullptr && newSelectedInputNode->selectHandler != nullptr)
		{
			if (newSelectedInputNode->selectHandler->IsSelectable())
			{
				if (selectedNode != nullptr)
					selectedNode->selectHandler->OnEvent(UISelectEventHandler::DeselectedEvent());

				selectedNode = newSelectedInputNode;

				selectedNode->selectHandler->OnEvent(UISelectEventHandler::SelectedEvent());
			}
		}
		else
		{
			if (selectedNode != nullptr)
				selectedNode->selectHandler->OnEvent(UISelectEventHandler::DeselectedEvent());

			selectedNode = nullptr;
		}
	}
	void InputManager::SubscribeToScreen(Screen& screen)
	{		
		screen.nodeTreeChangedEventDispatcher.AddHandler(*this);
		screen.screenDestructionEventDispatcher.AddHandler(*this);

		if (screen.GetWindow() != nullptr)
			SubscribeToWindow(*screen.GetWindow());		
	}
	void InputManager::UnsubscribeFromScreen(Screen& screen)
	{		
		screen.nodeTreeChangedEventDispatcher.AddHandler(*this);
		screen.screenDestructionEventDispatcher.AddHandler(*this);

		if (screen.GetWindow() != nullptr)
			UnsubscribeFromWindow(*screen.GetWindow());		
	}
	void InputManager::SubscribeToWindow(WindowBase& window)
	{		
		window.mouseMotionDispatcher.AddHandler(*this);
		window.mouseScrollDispatcher.AddHandler(*this);
		window.keyPressedDispatcher.AddHandler(*this);
		window.keyReleasedDispatcher.AddHandler(*this);
		window.textInputDispatcher.AddHandler(*this);	

		UpdateScreenMousePos();
	}
	void InputManager::UnsubscribeFromWindow(WindowBase& window)
	{
		window.mouseMotionDispatcher.AddHandler(*this);
		window.mouseScrollDispatcher.AddHandler(*this);
		window.keyPressedDispatcher.AddHandler(*this);
		window.keyReleasedDispatcher.AddHandler(*this);
		window.textInputDispatcher.AddHandler(*this);
	}
	void InputManager::UpdateScreenMousePos()
	{
		auto screenFinalTransform = screen->GetFinalTransform();
		screenMousePos = (screenFinalTransform.TransformFromFinalToLocalTransformSpace((Vec2f)screen->GetWindow()->GetMousePos()) - screenFinalTransform.position) * screenFinalTransform.size / (Vec2f)screen->GetWindow()->GetSize();
	}
	void InputManager::GatherInputNodes(Node& node)
	{
		if (InputNode* inputNode = dynamic_cast<InputNode*>(&node))
			nodesData.AddBack(InputNodeData{ false, *inputNode, InputNodeFinalTransformUpdatedEventHandler(this), InputNodeEnabledStateUpdatedEventHandler(this)}) ;
		
		for (auto& child : node.GetChildren())
			GatherInputNodes(child);
	}
	void InputManager::RecreateScreenInputNodes()
	{		
		nodesData.Clear();		

		for (auto& child : screen->GetChildren())
			GatherInputNodes(child);

		for (uintMem i = 0; i < nodesData.Count(); ++i)
		{
			auto& nodeData = nodesData[i];						
			nodeData.node.finalTransformUpdatedEventDispatcher.AddHandler(nodeData.finalTransformUpdatedEventHandler);

			if (nodeData.node.mouseHandler == nullptr)
				nodeData.node.mouseHandler = dynamic_cast<UIMouseEventHandler*>(&nodeData.node);
			if (nodeData.node.keyboardHandler == nullptr)
				nodeData.node.keyboardHandler = dynamic_cast<UIKeyboardEventHandler*>(&nodeData.node);
			if (nodeData.node.selectHandler == nullptr)
				nodeData.node.selectHandler = dynamic_cast<UISelectEventHandler*>(&nodeData.node);			
		}

		recreateScreenNodes = false;
	}
	void InputManager::CastMousePointer(uintMem beginIndex, Vec2f mouseDelta)
	{
		if (nodesData.Empty())
			return;

		if (beginIndex >= mouseBlockInputNodeIndex)
		{
			Debug::Breakpoint();
			return;
		}

		bool blocked = false;
		bool notifiedSelectedNode = false;

		uintMem i = beginIndex + 1;
		while (true)
		{
			--i;

			auto& nodeData = nodesData[i];
			auto& node = nodeData.node;

			if (!node.IsEnabled())
				continue;

			bool oldHit = nodeData.hit;
			bool newHit = node.HitTest(screenMousePos);
			nodeData.hit = newHit;

			if (oldHit && !newHit)
			{
				if (auto mouseEventHandler = node.mouseHandler)
					mouseEventHandler->OnEvent(UIMouseEventHandler::MouseExitEvent({
						.inputManager = this,
						.pos = screenMousePos
						}));
			}

			if (blocked)
				nodeData.hit = false;
			else
			{
				if (newHit)
				{
					if (auto mouseEventHandler = node.mouseHandler)
						mouseEventHandler->OnEvent(UIMouseEventHandler::MouseMotionEvent{
								.inputManager = this,
								.pos = screenMousePos,
								.delta = mouseDelta
							});

					if (selectedNode == &node)
						notifiedSelectedNode = true;
				}

				if (newHit && !oldHit)
				{
					if (auto mouseEventHandler = node.mouseHandler)
						mouseEventHandler->OnEvent(UIMouseEventHandler::MouseEnterEvent({
								.inputManager = this,
								.pos = screenMousePos
							}));
				}

				if (newHit)
				{
					if (node.blocksMouseEvents)
					{
						mouseBlockInputNodeIndex = i;
						blocked = true;
					}
				}
			}							


			if (i == 0)
				break;
		}

		//If the mouse ray wasn't blocked set the index to outside the array
		mouseBlockInputNodeIndex = nodesData.Count();		

		if (!notifiedSelectedNode && selectedNode != nullptr)
			if (auto mouseEventHandler = selectedNode->mouseHandler)
				mouseEventHandler->OnEvent(UIMouseEventHandler::MouseMotionEvent{
						.inputManager = this,
						.pos = screenMousePos,
						.delta = mouseDelta
					});
	}
	void InputManager::CastMousePointer(Node* startNode, Vec2f mouseDelta)
	{				
		castMousePointerBFSBuffer.Resize(screen->GetNodeCount());		
	}
	void InputManager::NodeFinalTransformUpdated(InputNode& node)
	{			
		uintMem nodeDataIndex = 0;

		for (; nodeDataIndex < nodesData.Count(); ++nodeDataIndex)
			if (&nodesData[nodeDataIndex].node == &node)
				break;

		if (nodeDataIndex >= nodesData.Count())
		{
			Debug::Logger::LogError("Blaze Engine Graphics", "Input node data count not be found in UI input manager!");
			return;
		}

		if (mouseBlockInputNodeIndex > nodeDataIndex)
			return;		

		InputNodeData& nodeData = nodesData[nodeDataIndex];		

		bool oldHit = nodeData.hit;
		bool newHit = node.HitTest(screenMousePos);
		nodeData.hit = newHit;

		
		if (oldHit && !newHit)
		{
			//If the mouse on its way to this node wasn't blocked send an event that the mouse exited			
			if (auto mouseEventHandler = node.mouseHandler)
				mouseEventHandler->OnEvent(UIMouseEventHandler::MouseExitEvent({
					.inputManager = this,
					.pos = screenMousePos
					}));

			//If this node was blocking the mouse cast the mouse pointer further
			if (mouseBlockInputNodeIndex == nodeDataIndex && mouseBlockInputNodeIndex != 0)
				CastMousePointer(mouseBlockInputNodeIndex - 1, Vec2f());
		}
		else if (newHit && !oldHit && mouseBlockInputNodeIndex != 0)
			CastMousePointer(mouseBlockInputNodeIndex - 1, Vec2f());
	}
	void InputManager::NodeDisabled(InputNode& node)
	{
		if (selectedNode == &node)		
			SelectNode(nullptr);		
	}			
	void InputManager::OnEvent(NodeTreeChangedEvent event)
	{
		auto inputNode = dynamic_cast<UI::InputNode*>(&event.node);

		if (inputNode == nullptr)
			return;		

		if (event.type == NodeTreeChangedEvent::Type::NodeRemoved)
			if (selectedNode == inputNode)
				SelectNode(nullptr);					

		recreateScreenNodes = true;
	}
	void InputManager::OnEvent(ScreenDestructionEvent event)
	{
		SetScreen(nullptr);		
	}
	void InputManager::OnEvent(ScreenWindowChangedEvent event)
	{
		if (event.oldWindow != nullptr)
			UnsubscribeFromWindow(*event.oldWindow);		

		UpdateScreenMousePos();

		if (screen->GetWindow() != nullptr)
			SubscribeToWindow(*screen->GetWindow());		
	}			
	void InputManager::OnEvent(Input::Events::MouseMotion event)
	{		
		UpdateScreenMousePos();

		if (recreateScreenNodes)
			RecreateScreenInputNodes();


		mouseBlockInputNodeIndex = nodesData.Count();		

		CastMousePointer(nodesData.Count() - 1, (Vec2f)event.delta * screen->GetFinalTransform().size / (Vec2f)screen->GetWindow()->GetSize());
	}
	void InputManager::OnEvent(Input::Events::MouseScroll event)
	{				
		if (recreateScreenNodes)
			RecreateScreenInputNodes();		

		for (auto& nodeData : nodesData)
		{
			if (!nodeData.node.IsEnabled())
				continue;

			if (nodeData.hit)
			{
				if (nodeData.node.mouseHandler)					
					nodeData.node.mouseHandler->OnEvent(UIMouseEventHandler::MouseScrollEvent {
						.pos = screenMousePos,
						.value = event.value
					});

				if (nodeData.node.blocksMouseEvents)
					break;
			}
		}
	}
	void InputManager::OnEvent(Input::Events::KeyPressed event)
	{
		if (recreateScreenNodes)
			RecreateScreenInputNodes();

		UIMouseEventHandler::MouseButton button = (UIMouseEventHandler::MouseButton)((uint)event.key - (uint)Key::MouseLeft);			
		
		if ((uint)button < 3)
		{
			bool selectedNodeHit = false;
			auto oldSelectedNode = selectedNode;			

			for (uintMem i = 0; i < mouseBlockInputNodeIndex; ++i)
			{
				auto& nodeData = nodesData[i];

				if (!nodeData.node.IsEnabled())
					continue;

				if (nodeData.hit)
				{
					if (nodeData.node.mouseHandler != nullptr)
						nodeData.node.mouseHandler->OnEvent(UIMouseEventHandler::MousePressedEvent({
							.inputManager = this,
							.pos = screenMousePos,
							.button = button,
							.time = event.time,
							.combo = event.combo
						}));

					if (selectedNode == &nodeData.node)
						selectedNodeHit = true;

					if (nodeData.node.blocksMouseEvents)
						break;
				}
			}

			if (!selectedNodeHit && oldSelectedNode == selectedNode)
				SelectNode(nullptr);
		}
		else
			if (selectedNode != nullptr && selectedNode->keyboardHandler != nullptr)
			{
				selectedNode->keyboardHandler->OnEvent(UIKeyboardEventHandler::KeyPressedEvent({
					.inputManager = this,
					.key = event.key,
					.time = event.time,
					.combo = event.combo
					}));
			}
	}
	void InputManager::OnEvent(Input::Events::KeyReleased event)
	{
		if (recreateScreenNodes)
			RecreateScreenInputNodes();

		UIMouseEventHandler::MouseButton button = (UIMouseEventHandler::MouseButton)((uint)event.key - (uint)Key::MouseLeft);
		
		if ((uint)button < 3)
		{			
			bool blocked = false;

			if (selectedNode != nullptr)
			{
				if (selectedNode->mouseHandler != nullptr)
					selectedNode->mouseHandler->OnEvent(UIMouseEventHandler::MouseReleasedEvent({
						.inputManager = this,
						.pos = screenMousePos,
						.button = button,
						.time = event.time
						}));

				if (selectedNode->blocksMouseEvents)
					blocked = true;
			}

			if (!blocked)
				for (auto& nodeData : nodesData)
				{
					if (!nodeData.node.IsEnabled())
						continue;

					if (nodeData.hit)
					{
						if (&nodeData.node != selectedNode && nodeData.node.mouseHandler != nullptr)
							nodeData.node.mouseHandler->OnEvent(UIMouseEventHandler::MouseReleasedEvent({
							.inputManager = this,
								}));

						if (nodeData.node.blocksMouseEvents)
							break;
					}
				}
		}
		else
			if (selectedNode != nullptr && selectedNode->keyboardHandler != nullptr)
			{
				selectedNode->keyboardHandler->OnEvent(UIKeyboardEventHandler::KeyReleasedEvent({
					.inputManager = this,
					.key = event.key,
					.time = event.time,					
					}));
			}
	}
	void InputManager::OnEvent(Input::Events::TextInput event)
	{
		if (recreateScreenNodes)
			RecreateScreenInputNodes();

		if (selectedNode != nullptr && selectedNode->keyboardHandler != nullptr)
		{
			selectedNode->keyboardHandler->OnEvent(UIKeyboardEventHandler::TextInputEvent({
				.inputManager = this,
				.input = event.input
				}));
		}
	}
	InputManager::InputNodeFinalTransformUpdatedEventHandler::InputNodeFinalTransformUpdatedEventHandler(InputManager* inputManager) :
		inputManager(inputManager)
	{
	}
	void InputManager::InputNodeFinalTransformUpdatedEventHandler::OnEvent(Node::FinalTransformUpdatedEvent event)
	{				
		if (event.node.IsEnabled())			
			inputManager->NodeFinalTransformUpdated((InputNode&)event.node);
	}
	InputManager::InputNodeEnabledStateUpdatedEventHandler::InputNodeEnabledStateUpdatedEventHandler(InputManager* inputManager)
		: inputManager(inputManager)
	{
	}
	void InputManager::InputNodeEnabledStateUpdatedEventHandler::OnEvent(Node::EnabledStateUpdatedEvent event)
	{
		if (!event.node.IsEnabled())
			inputManager->NodeDisabled((InputNode&)event.node);
	}
}
