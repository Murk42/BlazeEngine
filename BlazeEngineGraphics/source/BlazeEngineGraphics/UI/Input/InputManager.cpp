#include "pch.h"
#include "BlazeEngineGraphics/UI/Input/InputManager.h"
#include "BlazeEngineGraphics/UI/Input/MouseEventHandler.h"
#include "BlazeEngineGraphics/UI/Input/KeyboardEventHandler.h"
#include "BlazeEngineGraphics/UI/Input/SelectEventHandler.h"
#include "BlazeEngine/Input/Input.h"
#include <ranges>

namespace Blaze::UI
{		
	InputManager::InputManager()
		: selectedNode(nullptr), screen(nullptr), blockingNode(0)
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
			SubscribeToScreen(*this->screen);		
	}
	void InputManager::SelectNode(InputNode* newSelectedInputNode)
	{
		if (selectedNode == newSelectedInputNode)
			return;

		if (selectedNode != nullptr && selectedNode->selectHandler != nullptr)
			selectedNode->selectHandler->OnEvent(UISelectEventHandler::DeselectedEvent{
				.inputManager = *this,
				.node = *selectedNode
				});

		if (newSelectedInputNode == nullptr)
		{
			selectedNode = nullptr;
			return;
		}

		if (!newSelectedInputNode->IsEnabled())
		{
			BLAZE_ENGINE_GRAPHICS_ERROR("Trying to select a disabled node");
			return;
		}
		
		if (newSelectedInputNode->selectHandler != nullptr && !newSelectedInputNode->selectHandler->IsSelectable())
			return;

		selectedNode = newSelectedInputNode;

		if (newSelectedInputNode->selectHandler != nullptr)
			newSelectedInputNode->selectHandler->OnEvent(UISelectEventHandler::SelectedEvent{
				.inputManager = *this,
				.node = *newSelectedInputNode
				});
	}
	void InputManager::SubscribeToScreen(Screen& screen)
	{		
		screen.nodeTreeChangedEventDispatcher.AddHandler(*this);
		screen.screenDestructionEventDispatcher.AddHandler(*this);		

		if (screen.GetWindow() != nullptr)
			SubscribeToWindow(*screen.GetWindow());		

		SetInputManagerRecursively(&screen, this);
	}
	void InputManager::UnsubscribeFromScreen(Screen& screen)
	{						
		screen.nodeTreeChangedEventDispatcher.RemoveHandler(*this);
		screen.screenDestructionEventDispatcher.RemoveHandler(*this);

		if (screen.GetWindow() != nullptr)
			UnsubscribeFromWindow(*screen.GetWindow());		

		SetInputManagerRecursively(&screen, nullptr);
	}
	void InputManager::SubscribeToWindow(Window& window)
	{			
		window.windowFocusLostEventDispatcher.AddHandler<&InputManager::Event_WindowFocusLost>(*this);
		window.mouseMotionEventDispatcher.AddHandler<&InputManager::Event_MouseMotion>(*this);
		window.mouseScrollEventDispatcher.AddHandler(*this);
		window.mouseButtonDownEventDispatcher.AddHandler(*this);
		window.mouseButtonUpEventDispatcher.AddHandler(*this);
		window.keyDownEventDispatcher.AddHandler(*this);
		window.keyUpEventDispatcher.AddHandler(*this);
		window.textInputEventDispatcher.AddHandler(*this);	

		UpdateScreenMousePos(window.GetMousePos());
	}
	void InputManager::UnsubscribeFromWindow(Window& window)
	{
		window.windowFocusLostEventDispatcher.RemoveHandler<&InputManager::Event_WindowFocusLost>(*this);
		window.mouseMotionEventDispatcher.RemoveHandler<&InputManager::Event_MouseMotion>(*this);
		window.mouseScrollEventDispatcher.RemoveHandler(*this);
		window.mouseButtonDownEventDispatcher.RemoveHandler(*this);
		window.mouseButtonUpEventDispatcher.RemoveHandler(*this);
		window.keyDownEventDispatcher.RemoveHandler(*this);
		window.keyUpEventDispatcher.RemoveHandler(*this);
		window.textInputEventDispatcher.RemoveHandler(*this);	
	}
	void InputManager::UpdateScreenMousePos(Vec2f windowPos)
	{		
		auto screenFinalTransform = screen->GetFinalTransform();
		screenMousePos = screenFinalTransform.TransformFromFinalToLocalTransformSpace(windowPos - screenFinalTransform.position) * screenFinalTransform.size / (Vec2f)screen->GetWindow()->GetSize();
	}	
	void InputManager::CastMousePointer(Vec2f delta, uint64 timeNS)
	{		
		bool blocked = false;
		bool notifiedSelectedNode = false;				
		
		Node* end = blockingNode == nullptr ? nullptr : blockingNode->GetPreviousNodeInTree();

		blockingNode = nullptr;
		
		for (Node* it = screen->GetTree().LastIterator().Ptr(); it != end; it = it->GetPreviousNodeInTree())
		{									
			if (!it->IsEnabled())
				continue;			
			
			int newHit = 0;

			if (!blocked)
			{
				newHit = it->HitTest(screenMousePos);

				if (newHit > 0)
				{
					blockingNode = it;
					blocked = true;
				}
			}

			InputNode* node = dynamic_cast<InputNode*>(it);

			if (node != nullptr)
			{
				int oldHit = node->hit;
				node->hit = newHit;

				if (!newHit && oldHit && node->mouseHandler != nullptr)
				{
					node->mouseHandler->OnEvent(UIMouseEventHandler::MouseExitEvent{
						.inputManager = *this,
						.node = *node,					
						.timeNS = timeNS						
						});
				}

				if (blocked && blockingNode != it)
					node->hit = 0;
				else
				{
					if (newHit)
					{
						if (node->mouseHandler != nullptr)
							node->mouseHandler->OnEvent(UIMouseEventHandler::MouseMotionEvent{							
									.inputManager = *this,
									.node = *node,
									.timeNS = timeNS,									
									.mouseID = 0, //TODO determine mouseID
									.delta = delta,
									.screenPos = screenMousePos,
									.windowPos = windowMousePos,
									.desktopPos = desktopMousePos,
								});

						if (selectedNode == node)
							notifiedSelectedNode = true;
					}

					if (newHit && !oldHit)
					{
						if (node->mouseHandler != nullptr)
							node->mouseHandler->OnEvent(UIMouseEventHandler::MouseEnterEvent {
									.inputManager = *this,
									.node = *node,
									.timeNS = timeNS
								});
					}					
				}
			}			
		}
		
		if (!notifiedSelectedNode && selectedNode != nullptr)
			if (selectedNode->mouseHandler != nullptr)
				selectedNode->mouseHandler->OnEvent(UIMouseEventHandler::MouseMotionEvent {
						.inputManager = *this,
						.node = *selectedNode,
						.timeNS = timeNS,
						.mouseID = 0, //TODO determine mouseID
						.delta = delta,
						.screenPos = screenMousePos,
						.windowPos = windowMousePos,
						.desktopPos = desktopMousePos,
					});
	}		
	void InputManager::NodeFinalTransformUpdated(InputNode& node)
	{					
		CastMousePointer(Vec2f(), 0);
	}
	void InputManager::NodeEnabledStateChanged(InputNode& node)
	{
		if (node.IsEnabled())
		{
			CastMousePointer(Vec2f(), 0);
		}
		else
		{
			if (selectedNode == &node)
				SelectNode(nullptr);
		}
	}			
	void InputManager::OnEvent(const NodeTreeChangedEvent& event)
	{
		auto inputNode = dynamic_cast<UI::InputNode*>(&event.node);

		switch (event.type)
		{
		case NodeTreeChangedEvent::Type::NodeRemoved:
			SetInputManagerRecursively(&event.node, nullptr);

			if (selectedNode == inputNode)
				SelectNode(nullptr);
			break;
		case NodeTreeChangedEvent::Type::NodeAdded:
			SetInputManagerRecursively(&event.node, this);
			break;
		default:
			break;
		}
	}
	void InputManager::OnEvent(const ScreenDestructionEvent& event)
	{
		SetScreen(nullptr);		
	}
	void InputManager::OnEvent(const ScreenWindowChangedEvent& event)
	{
		if (event.oldWindow != nullptr)
			UnsubscribeFromWindow(*event.oldWindow);		
		
		if (screen->GetWindow() != nullptr)					
			SubscribeToWindow(*screen->GetWindow());		
	}
	void InputManager::Event_WindowFocusLost(const Window::WindowFocusLostEvent& event)
	{
		SelectNode(nullptr);
	}
	void InputManager::Event_MouseMotion(const Mouse::MouseMotionEvent& event)
	{		
		UpdateScreenMousePos(event.pos);
		windowMousePos = event.pos;
		desktopMousePos = event.desktopPos;

		blockingNode = nullptr;

		CastMousePointer(event.delta, event.timeNS);
	}
	void InputManager::OnEvent(const Mouse::MouseScrollEvent& event)
	{					
		auto end = --NodeTreeIterator(blockingNode);
		for (auto it = screen->GetTree().LastIterator(); it != end; --it)
		{
			InputNode* node = dynamic_cast<InputNode*>(&*it);

			if (node == nullptr || !node->IsEnabled() || node->hit == 0)
				continue;

			if (node->mouseHandler)
				node->mouseHandler->OnEvent(UIMouseEventHandler::MouseScrollEvent{
					.inputManager = *this,
					.node = *node,
					.timeNS = event.timeNS,
					.mouseID = 0, //TODO determine mouseID
					.value = event.amount,
					.direction = event.direction,
					.screenPos = screenMousePos,
					.windowPos = event.pos,
					.desktopPos = event.desktopPos		
					});

			if (node->hit > 0)
				break;
		}
	}
	void InputManager::OnEvent(const Mouse::MouseButtonDownEvent& event)
	{		
		bool selectedNodeHit = false;
		auto oldSelectedNode = selectedNode;

		auto end = blockingNode == nullptr ? nullptr : blockingNode->GetPreviousNodeInTree();
		for (auto it = screen->GetTree().LastIterator(); it != end; --it)
		{
			InputNode* node = dynamic_cast<InputNode*>(&*it);

			if (node == nullptr || !node->IsEnabled() || node->hit == 0)
				continue;

			if (node->mouseHandler != nullptr)
				node->mouseHandler->OnEvent(UIMouseEventHandler::MouseButtonDownEvent{
					.inputManager = *this,
					.node = *node,
					.timeNS = event.timeNS,
					.mouseID = event.mouseID,
					.button = event.button,					
					.combo = event.combo,
					.screenPos = screenMousePos,
					.windowPos = event.pos,
					.desktopPos = event.desktopPos,															
					});

			if (selectedNode == node)
				selectedNodeHit = true;

			if (node->hit > 1)
				break;
		}

		if (!selectedNodeHit && oldSelectedNode == selectedNode)
			SelectNode(nullptr);
	}
	void InputManager::OnEvent(const Mouse::MouseButtonUpEvent& event)
	{
		bool selectedNodeAcceptedEvent = false;

		if (selectedNode != nullptr)
		{
			if (selectedNode->mouseHandler != nullptr)
				selectedNode->mouseHandler->OnEvent(UIMouseEventHandler::MouseButtonUpEvent{
					.inputManager = *this,
					.node = *selectedNode,
					.timeNS = event.timeNS,
					.mouseID = event.mouseID,
					.button = event.button,
					.combo = event.combo,
					.screenPos = screenMousePos,
					.windowPos = event.pos,
					.desktopPos = event.desktopPos,					
				});

			if (selectedNode == blockingNode)
				selectedNodeAcceptedEvent = true;
		}

		if (!selectedNodeAcceptedEvent)
		{
			auto end = blockingNode == nullptr ? nullptr : blockingNode->GetPreviousNodeInTree();
			for (auto it = screen->GetTree().LastIterator(); it != end; --it)
			{
				InputNode* node = dynamic_cast<InputNode*>(&*it);

				if (node == nullptr || !node->IsEnabled() || node->hit == 0)
					continue;

				if (node != selectedNode && node->mouseHandler != nullptr)
					node->mouseHandler->OnEvent(UIMouseEventHandler::MouseButtonUpEvent{
						.inputManager = *this,
						.node = *node,
						.timeNS = event.timeNS,
						.mouseID = event.mouseID,
						.button = event.button,
						.combo = event.combo,
						.screenPos = screenMousePos,
						.windowPos = event.pos,
						.desktopPos = event.desktopPos,
					});

				if (node->hit > 0)
					break;;
			}
		}
	}
	void InputManager::OnEvent(const Keyboard::KeyDownEvent& event)
	{									
		if (selectedNode == nullptr)
			return;

		if (selectedNode->keyboardHandler == nullptr)
			return;

		selectedNode->keyboardHandler->OnEvent(UIKeyboardEventHandler::KeyDownEvent{
				.inputManager = *this,
				.node = *selectedNode,
				.timeNS = event.timeNS,
				.keyboardID = event.keyboardID,
				.key = event.key,
				.modifier = event.modifier,
				.keyCode = event.keyCode,
				.repeat = event.repeat,				
			});		
	}
	void InputManager::OnEvent(const Keyboard::KeyUpEvent& event)
	{
		if (selectedNode == nullptr)
			return;

		if (selectedNode->keyboardHandler == nullptr)
			return;

		selectedNode->keyboardHandler->OnEvent(UIKeyboardEventHandler::KeyUpEvent{
				.inputManager = *this,
				.node = *selectedNode,
				.timeNS = event.timeNS,
				.keyboardID = event.keyboardID,
				.key = event.key,
				.modifier = event.modifier,
				.keyCode = event.keyCode,				
			});
	}
	void InputManager::OnEvent(const Keyboard::TextInputEvent& event)
	{		
		if (selectedNode == nullptr)
			return;		

		if (selectedNode->keyboardHandler == nullptr)
			return;

		selectedNode->keyboardHandler->OnEvent(UIKeyboardEventHandler::TextInputEvent{
			.inputManager = *this,
			.node = *selectedNode,
			.string = event.string
			});
	}
	void InputManager::SetInputManagerRecursively(Node* node, InputManager* ptr)
	{
		if (InputNode* inputNode = dynamic_cast<InputNode*>(node))
		{
			inputNode->inputManager = ptr;

			if (ptr != nullptr)
			{
				if (inputNode->mouseHandler == nullptr)
					inputNode->mouseHandler = dynamic_cast<UIMouseEventHandler*>(inputNode);
				if (inputNode->keyboardHandler == nullptr)
					inputNode->keyboardHandler = dynamic_cast<UIKeyboardEventHandler*>(inputNode);
				if (inputNode->selectHandler == nullptr)
					inputNode->selectHandler = dynamic_cast<UISelectEventHandler*>(inputNode);
			}
		}

		for (auto& child : node->GetChildren())
			SetInputManagerRecursively(&child, ptr);
	}

}
