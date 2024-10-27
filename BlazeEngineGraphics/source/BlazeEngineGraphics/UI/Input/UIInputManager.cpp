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
		: selectedNode(nullptr), screen(nullptr), mouseBlockInputNodeIndex(0)
	{				
	}
	InputManager::~InputManager()
	{
		if (screen != nullptr)
			SubscribeToScreen(*screen);
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

	void InputManager::SelectNode(nullptr_t null)
	{
		SelectNode((InputNode*)null);
	}
	void InputManager::SelectNode(Node* newSelectedNode)
	{
		auto newSelectedInputNode = dynamic_cast<InputNode*>(newSelectedNode);

		SelectNode(newSelectedInputNode);
	}
	void InputManager::SelectNode(InputNode* newSelectedInputNode)
	{
		if (selectedNode == newSelectedInputNode)
			return;


		if (newSelectedInputNode != nullptr && newSelectedInputNode->selectHandler != nullptr)
		{
			if (newSelectedInputNode->selectHandler->selectable)
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
		screen.nodeCreatedEventDispatcher.AddHandler(*this);
		screen.nodeDestroyedEventDispatcher.AddHandler(*this);
		screen.screenDestructionEventDispatcher.AddHandler(*this);

		if (screen.GetWindow() != nullptr)
			SubscribeToWindow(*screen.GetWindow());		
	}
	void InputManager::UnsubscribeFromScreen(Screen& screen)
	{
		screen.nodeCreatedEventDispatcher.AddHandler(*this);
		screen.nodeDestroyedEventDispatcher.AddHandler(*this);
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
	}
	void InputManager::UnsubscribeFromWindow(WindowBase& window)
	{
		window.mouseMotionDispatcher.AddHandler(*this);
		window.mouseScrollDispatcher.AddHandler(*this);
		window.keyPressedDispatcher.AddHandler(*this);
		window.keyReleasedDispatcher.AddHandler(*this);
		window.textInputDispatcher.AddHandler(*this);
	}
	void InputManager::GatherInputNodes(Node& node)
	{
		if (InputNode* inputNode = dynamic_cast<InputNode*>(&node))
			nodesData.AddBack(InputNodeData{ *inputNode, InputNodeFinalTransformUpdatedEventHandler(this) });
		
		for (auto& child : node.GetChildren())
			GatherInputNodes(child);
	}
	void InputManager::RecreateScreenInputNodes()
	{
		nodesData.Clear();
		nodesData.ReserveExactly(screen->GetNodeCount());

		for (auto& child : screen->GetChildren())
			GatherInputNodes(child);

		for (uintMem i = 0; i < nodesData.Count(); ++i)
		{
			auto& nodeData = nodesData[i];			
			nodeData.node.inputManagerArrayIndex = i;
			nodeData.node.finalTransformUpdatedEventDispatcher.AddHandler(nodeData.finalTransformUpdatedEventHandler);

			if (nodeData.node.mouseHandler == nullptr)
				nodeData.node.mouseHandler = dynamic_cast<UIMouseEventHandler*>(&nodeData.node);
			if (nodeData.node.keyboardHandler == nullptr)
				nodeData.node.keyboardHandler = dynamic_cast<UIKeyboardEventHandler*>(&nodeData.node);
			if (nodeData.node.selectHandler == nullptr)
				nodeData.node.selectHandler = dynamic_cast<UISelectEventHandler*>(&nodeData.node);

			if (nodeData.node.mouseHandler != nullptr)
				nodeData.node.mouseHandler->lastTransformState = nodeData.node.GetTransformState();
		}

		recreateScreenNodes = false;
	}

	void InputManager::CastMousePointer(uintMem beginIndex, Vec2f mousePos, Vec2f mouseDelta)
	{
		if (nodesData.Empty())
			return;

		if (beginIndex >= mouseBlockInputNodeIndex)
		{
			Debug::Breakpoint();
			return;
		}

		bool blocked = false;

		uintMem i;
		for (i = beginIndex; i < nodesData.Count(); ++i)
		{
			auto& node = nodesData[i].node;

			bool oldHit = node.hit;
			bool newHit = node.HitTest(mousePos);
			node.hit = newHit;

			if (oldHit && !newHit)
			{
				if (auto mouseEventHandler = node.mouseHandler)
					mouseEventHandler->OnEvent(UIMouseEventHandler::MouseExitEvent({
						.inputManager = this,
						.pos = mousePos
						}));
			}

			if (blocked)
				nodesData[i].node.hit = false;
			else
			{
				if (newHit)
				{
					if (auto mouseEventHandler = node.mouseHandler)
						mouseEventHandler->OnEvent(UIMouseEventHandler::MouseMotionEvent{
								.inputManager = this,
								.pos = mousePos,
								.delta = mouseDelta
							});
				}

				if (newHit && !oldHit)
				{
					if (auto mouseEventHandler = node.mouseHandler)
						mouseEventHandler->OnEvent(UIMouseEventHandler::MouseEnterEvent({
								.inputManager = this,
								.pos = mousePos
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
		}

		//If the mouse ray wasn't blocked set the index to outside the array
		mouseBlockInputNodeIndex = nodesData.Count();		
	}

	void InputManager::NodeFinalTransformUpdated(InputNode& node)
	{
		if (mouseBlockInputNodeIndex > node.inputManagerArrayIndex)
			return;

		Vec2f mousePos = Vec2f(screen->GetWindow()->GetMousePos());

		bool oldHit = node.hit;
		bool newHit = node.HitTest(mousePos);
		node.hit = newHit;
		
		if (oldHit && !newHit)
		{
			//If the mouse on its way to this node wasn't blocked send an event that the mouse exited			
			if (auto mouseEventHandler = node.mouseHandler)
				mouseEventHandler->OnEvent(UIMouseEventHandler::MouseExitEvent({
					.inputManager = this,
					.pos = mousePos
					}));

			//If this node was blocking the mouse cast the mouse pointer further
			if (mouseBlockInputNodeIndex == node.inputManagerArrayIndex)
				CastMousePointer(node.inputManagerArrayIndex + 1, mousePos, Vec2f());
		}

		if (newHit && !oldHit)		
			CastMousePointer(mouseBlockInputNodeIndex + 1, mousePos, Vec2f());
	}
		
	void InputManager::OnEvent(NodeCreatedEvent event)
	{				
		recreateScreenNodes = true;
	}
	void InputManager::OnEvent(NodeDestroyedEvent event)
	{
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

		if (screen->GetWindow() != nullptr)
			SubscribeToWindow(*screen->GetWindow());		
	}			
	void InputManager::OnEvent(Input::Events::MouseMotion event)
	{		
		if (recreateScreenNodes)
			RecreateScreenInputNodes();

		mouseBlockInputNodeIndex = nodesData.Count();

		CastMousePointer(0, (Vec2f)screen->GetWindow()->GetMousePos(), (Vec2f)event.delta);
	}
	void InputManager::OnEvent(Input::Events::MouseScroll event)
	{				
		if (recreateScreenNodes)
			RecreateScreenInputNodes();

		Vec2f mousePos = Vec2f(screen->GetWindow()->GetMousePos());

		for (auto& nodeData : nodesData)
		{
			if (nodeData.node.hit)
			{
				if (nodeData.node.mouseHandler)					
					nodeData.node.mouseHandler->OnEvent(UIMouseEventHandler::MouseScrollEvent {
						.pos = mousePos,
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
			
			Vec2f mousePos = Vec2f(screen->GetWindow()->GetMousePos());

			for (uintMem i = 0; i < mouseBlockInputNodeIndex; ++i)
			{
				auto& nodeData = nodesData[i];

				if (nodeData.node.hit)
				{
					if (nodeData.node.mouseHandler != nullptr)
						nodeData.node.mouseHandler->OnEvent(UIMouseEventHandler::MousePressedEvent({
							.inputManager = this,
							.pos = mousePos,
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
			Vec2f mousePos = Vec2f(screen->GetWindow()->GetMousePos());
			bool blocked = false;

			if (selectedNode != nullptr)
			{
				if (selectedNode->mouseHandler != nullptr)
					selectedNode->mouseHandler->OnEvent(UIMouseEventHandler::MouseReleasedEvent({
						.inputManager = this,
						.pos = mousePos,
						.button = button,
						.time = event.time
						}));

				if (selectedNode->blocksMouseEvents)
					blocked = true;
			}

			if (!blocked)
				for (auto& nodeData : nodesData)
					if (nodeData.node.hit)
					{
						if (&nodeData.node != selectedNode && nodeData.node.mouseHandler != nullptr)
							nodeData.node.mouseHandler->OnEvent(UIMouseEventHandler::MouseReleasedEvent({
							.inputManager = this,
							}));

						if (nodeData.node.blocksMouseEvents)
							break;
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
		inputManager->NodeFinalTransformUpdated((InputNode&)event.node);
	}
}
