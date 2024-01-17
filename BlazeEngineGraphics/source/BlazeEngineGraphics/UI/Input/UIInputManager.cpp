#include "pch.h"
#include "BlazeEngineGraphics/UI/Input/UIInputManager.h"
#include "BlazeEngineGraphics/UI/Input/UIMouseEventHandler.h"
#include "BlazeEngineGraphics/UI/Input/UIKeyboardEventHandler.h"
#include "BlazeEngineGraphics/UI/Input/UISelectEventHandler.h"
#include "BlazeEngine/Input/Input.h"

namespace Blaze::UI
{
	UIInputManager::UIInputManager()
		: selectedNode(nullptr), recreateInputNodes(false), screen(nullptr), window(nullptr)
	{		
		Input::GetInputEventSystem().inputPreUpdateDispatcher.AddHandler(*this);
		Input::GetInputEventSystem().inputPostUpdateDispatcher.AddHandler(*this);
	}
	UIInputManager::~UIInputManager()
	{		
	}
	void UIInputManager::SetWindow(WindowBase* newWindow)
	{
		if (window != nullptr)
		{
			window->mouseMotionDispatcher.RemoveHandler(*this);
			window->mouseScrollDispatcher.RemoveHandler(*this);
			window->keyPressedDispatcher.RemoveHandler(*this);
			window->keyReleasedDispatcher.RemoveHandler(*this);
			window->textInputDispatcher.RemoveHandler(*this);			
		}

		window = newWindow;

		if (window != nullptr)
		{
			window->mouseMotionDispatcher.AddHandler(*this);
			window->mouseScrollDispatcher.AddHandler(*this);
			window->keyPressedDispatcher.AddHandler(*this);
			window->keyReleasedDispatcher.AddHandler(*this);
			window->textInputDispatcher.AddHandler(*this);
		}		
	}
	void UIInputManager::SetScreen(Screen* newScreen)
	{
		if (screen != nullptr)
		{
			screen->nodeCreatedEventDispatcher.RemoveHandler(*this);
			screen->nodeDestroyedEventDispatcher.RemoveHandler(*this);
			screen->screenDestructionEventDispatcher.RemoveHandler(*this);
		}

		screen = newScreen;

		if (screen != nullptr)
		{
			recreateInputNodes = true;

			screen->nodeCreatedEventDispatcher.AddHandler(*this);
			screen->nodeDestroyedEventDispatcher.AddHandler(*this);
			screen->screenDestructionEventDispatcher.AddHandler(*this);
		}	
		else
		{
			nodes.Clear();
		}
	}

	void UIInputManager::SelectNode(nullptr_t null)
	{
		SelectNode((UIInputNode*)null);
	}
	void UIInputManager::SelectNode(Node* newSelectedNode)
	{		
		auto newSelectedInputNode = dynamic_cast<UIInputNode*>(newSelectedNode);

		SelectNode(newSelectedInputNode);
	}
	void UIInputManager::SelectNode(UIInputNode* newSelectedInputNode)
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

	static void GatherInputNodes(Node* node, Array<UIInputNode*>& arr)
	{
		if (auto inputNode = dynamic_cast<UIInputNode*>(node))
		{			
			arr.AddBack(inputNode);
		}

		for (auto child : node->GetChildren())
			GatherInputNodes(child, arr);
	}	

	void UIInputManager::RecreateInputNodes()
	{
		nodes.Clear();

		for (auto child : screen->GetChildren())
			GatherInputNodes(child, nodes);		

		for (auto node : nodes)
		{
			if (node->mouseHandler == nullptr)			
				node->mouseHandler = dynamic_cast<UIMouseEventHandler*>(node);			
			if (node->keyboardHandler == nullptr)
				node->keyboardHandler = dynamic_cast<UIKeyboardEventHandler*>(node);
			if (node->selectHandler == nullptr)
				node->selectHandler = dynamic_cast<UISelectEventHandler*>(node);			

			if (node->mouseHandler != nullptr)
				node->mouseHandler->lastTransformState = node->GetTransformState();
		}

		recreateInputNodes = false;
	}
	void UIInputManager::CheckMousePositionEvents(Vec2i delta)
	{
		auto oldSelectedNode = selectedNode;

		bool alreadyHit = false;
		for (auto node : nodes)
		{
			bool oldHit = node->hit;
			bool newHit = node->HitTest(mousePos) && !alreadyHit;
			node->hit = newHit;

			if (auto mouseEventHandler = node->mouseHandler)
			{				
				if (newHit && !oldHit)
					mouseEventHandler->OnEvent(UIMouseEventHandler::MouseEnterEvent({
						.inputManager = this,
						}));
				
				if (oldHit && !newHit)
					mouseEventHandler->OnEvent(UIMouseEventHandler::MouseExitEvent({
						.inputManager = this,
						}));				

				uint32 lastState = node->GetTransformState();

				if ((delta != Vec2i() || mouseEventHandler->lastTransformState != lastState) && (newHit || oldSelectedNode == node))
				{
					UIMouseEventHandler::MouseMotionEvent _event{
						.inputManager = this,
						.pos = mousePos,
						.delta = Vec2f(delta)
					};
					mouseEventHandler->OnEvent(_event);
				}

				mouseEventHandler->lastTransformState = lastState;
			}

			if (!node->hitPropagates && newHit)
				alreadyHit = true;
		}
	}		
	void UIInputManager::OnEvent(NodeCreatedEvent event)
	{
		if (auto inputNode = dynamic_cast<UIInputNode*>(event.node))
			recreateInputNodes = true;		
	}
	void UIInputManager::OnEvent(NodeDestroyedEvent event)
	{
		if (auto inputNode = dynamic_cast<UIInputNode*>(event.node))
			recreateInputNodes = true;		
	}
	void UIInputManager::OnEvent(ScreenDestructionEvent event)
	{
		SetScreen(nullptr);
	}
	void UIInputManager::OnEvent(Input::Events::InputPreUpdateEvent event)
	{		
		if (window == nullptr)
			return;
		
		if (recreateInputNodes)
			RecreateInputNodes();		

		mousePos = (Vec2f)window->GetMousePos();						
	}
	void UIInputManager::OnEvent(Input::Events::InputPostUpdateEvent event)
	{
		if (window == nullptr)
			return;		

		CheckMousePositionEvents(Vec2i());
	}
	void UIInputManager::OnEvent(Input::Events::MouseMotion event)
	{				
		CheckMousePositionEvents(event.delta);		
	}
	void UIInputManager::OnEvent(Input::Events::MouseScroll event)
	{
		for (auto node : nodes)
		{
			if (node->hit && node->mouseHandler)
			{
				UIMouseEventHandler::MouseScrollEvent _event{
					.pos = mousePos,
					.value = event.value
				};
				node->mouseHandler->OnEvent(_event);

			}

			if (!node->hitPropagates && node->hit)
				break;
		}
	}
	void UIInputManager::OnEvent(Input::Events::KeyPressed event)
	{
		UIMouseEventHandler::MouseButton button = (UIMouseEventHandler::MouseButton)((uint)event.key - (uint)Key::MouseLeft);			
		
		if ((uint)button < 3)
		{
			bool selectedNodeHit = false;
			auto oldSelectedNode = selectedNode;

			for (auto node : nodes)
				if (node->hit)
				{				
					if (selectedNode == node)
						selectedNodeHit = true;

					if (node->mouseHandler != nullptr)
						node->mouseHandler->OnEvent(UIMouseEventHandler::MousePressedEvent({
							.inputManager = this,
							.pos = mousePos,
							.button = button,
							.time = event.time,
							.combo = event.combo
							}));

					if (!node->hitPropagates)
						break;
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
	void UIInputManager::OnEvent(Input::Events::KeyReleased event)
	{
		UIMouseEventHandler::MouseButton button = (UIMouseEventHandler::MouseButton)((uint)event.key - (uint)Key::MouseLeft);
		
		if ((uint)button < 3)
		{
			if (selectedNode != nullptr && selectedNode->mouseHandler != nullptr)			
				selectedNode->mouseHandler->OnEvent(UIMouseEventHandler::MouseReleasedEvent({
					.inputManager = this,
					.pos = mousePos,
					.button = button,
					.time = event.time
					}));							

			for (auto node : nodes)
				if (node->hit && node != selectedNode)
				{					
					if (node->mouseHandler != nullptr)
						node->mouseHandler->OnEvent(UIMouseEventHandler::MouseReleasedEvent({
							.inputManager = this,
							}));

					if (!node->hitPropagates)
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
	void UIInputManager::OnEvent(Input::Events::TextInput event)
	{
		if (selectedNode != nullptr && selectedNode->keyboardHandler != nullptr)
		{
			selectedNode->keyboardHandler->OnEvent(UIKeyboardEventHandler::TextInputEvent({
				.inputManager = this,
				.input = event.input
				}));
		}
	}
}