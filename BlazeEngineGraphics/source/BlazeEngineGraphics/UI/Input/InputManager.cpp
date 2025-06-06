#include "pch.h"
#include "BlazeEngineGraphics/UI/Input/InputManager.h"
#include "BlazeEngineCore/Debug/Logger.h"
#include "BlazeEngineCore/DataStructures/List.h"
#include "BlazeEngine/Input/Input.h"
#include <ranges>

namespace Blaze::UI
{			
	void InputManager::SelectNode(InputNode* newSelectedInputNode)
	{
		InputNode* oldSelectedNode = selectedNode;

		if (oldSelectedNode == newSelectedInputNode)
			return;						

		if (newSelectedInputNode == nullptr)
		{
			selectedNode = nullptr;

			if (oldSelectedNode != nullptr)
				oldSelectedNode->selectedStateChangedEventDispatcher.Call({
					.node = *selectedNode
					});

			return;
		}

		if (!newSelectedInputNode->IsEnabled())
		{
			BLAZE_ENGINE_GRAPHICS_ERROR("Trying to select a disabled node");
			return;
		}
		
		if (!newSelectedInputNode->IsSelectable())
			return;

		selectedNode = newSelectedInputNode;

		if (oldSelectedNode != nullptr)
			oldSelectedNode->selectedStateChangedEventDispatcher.Call({
				.node = *selectedNode
			});
		
		newSelectedInputNode->selectedStateChangedEventDispatcher.Call({
			.node = *newSelectedInputNode
			});
	}
	void InputManager::UpdateNodeHitStatus(InputNode& node)
	{
		//TODO optimize this
		CastMousePointer(Vec2f(), 0);		
	}
	InputManager* InputManager::GetScreenInputManager(Screen& screen)
	{
		auto it = screen.dataMap.map.Find("InputManager");		
		auto ptr = it.GetValue<InputManager>();
		return ptr;		
	}
	InputManager& InputManager::CreateScreenInputManager(Screen& screen)
	{
		auto [it, inserted, inputManager] = screen.dataMap.map.Insert<InputManager>("InputManager", screen);
		
		return *inputManager;
	}		
	InputManager::InputManager(Screen& screen)
		: selectedNode(nullptr), screen(screen), blockingNode(0)
	{
		if (screen.GetWindow() != nullptr)
			SubscribeToWindow(*screen.GetWindow());

		screen.screenWindowChangedEventDispatcher.AddHandler<&InputManager::ScreenWindowChangedEvent>(*this);
	}
	InputManager::~InputManager()
	{
		screen.screenWindowChangedEventDispatcher.RemoveHandler<&InputManager::ScreenWindowChangedEvent>(*this);		

		if (screen.GetWindow() != nullptr)
			UnsubscribeFromWindow(*screen.GetWindow());
	}
	void InputManager::SubscribeToWindow(Window& window)
	{
		window.windowFocusLostEventDispatcher.AddHandler<&InputManager::WindowFocusLostEvent>(*this);
		window.mouseMotionEventDispatcher.AddHandler<&InputManager::MouseMotionEvent>(*this);
		window.mouseScrollEventDispatcher.AddHandler<&InputManager::MouseScrollEvent>(*this);
		window.mouseButtonDownEventDispatcher.AddHandler<&InputManager::MouseButtonDownEvent>(*this);
		window.mouseButtonUpEventDispatcher.AddHandler<&InputManager::MouseButtonUpEvent>(*this);
		window.keyDownEventDispatcher.AddHandler<&InputManager::KeyDownEvent>(*this);
		window.keyUpEventDispatcher.AddHandler<&InputManager::KeyUpEvent>(*this);
		window.textInputEventDispatcher.AddHandler<&InputManager::TextInputEvent>(*this);

		UpdateScreenMousePos(window.GetMousePos());
	}
	void InputManager::UnsubscribeFromWindow(Window& window)
	{
		window.windowFocusLostEventDispatcher.RemoveHandler<&InputManager::WindowFocusLostEvent>(*this);
		window.mouseMotionEventDispatcher.RemoveHandler<&InputManager::MouseMotionEvent>(*this);
		window.mouseScrollEventDispatcher.RemoveHandler<&InputManager::MouseScrollEvent>(*this);
		window.mouseButtonDownEventDispatcher.RemoveHandler<&InputManager::MouseButtonDownEvent>(*this);
		window.mouseButtonUpEventDispatcher.RemoveHandler<&InputManager::MouseButtonUpEvent>(*this);
		window.keyDownEventDispatcher.RemoveHandler<&InputManager::KeyDownEvent>(*this);
		window.keyUpEventDispatcher.RemoveHandler<&InputManager::KeyUpEvent>(*this);
		window.textInputEventDispatcher.RemoveHandler<&InputManager::TextInputEvent>(*this);
	}
	void InputManager::UpdateScreenMousePos(Vec2f windowPos)
	{		
		auto screenFinalTransform = screen.GetFinalTransform();
		screenMousePos = screenFinalTransform.TransformFromFinalToLocalTransformSpace(windowPos - screenFinalTransform.position) * screenFinalTransform.size / (Vec2f)screen.GetWindow()->GetSize();
	}	
	void InputManager::CastMousePointer(Vec2f delta, uint64 timeNS)
	{		
		/*
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

			if (node == nullptr)
				continue;			

			int oldHit = node->mouseHitStatus;
			node->mouseHitStatus = newHit;

			if (!newHit && oldHit && node->mouseHandler != nullptr)
			{
				node->mouseHandler->OnEvent(UIMouseEventHandler::MouseExitEvent{
					.node = *node,
					.timeNS = timeNS
					});
			}

			if (blocked && blockingNode != it)
				node->mouseHitStatus = 0;
			else
			{
				if (newHit)
				{
					if (node->mouseHandler != nullptr)
						node->mouseHandler->OnEvent(UIMouseEventHandler::MouseMotionEvent{
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
						node->mouseHandler->OnEvent(UIMouseEventHandler::MouseEnterEvent{								
								.node = *node,
								.timeNS = timeNS
							});
				}
			}
		}
		
		if (!notifiedSelectedNode && selectedNode != nullptr)
			if (selectedNode->mouseHandler != nullptr)
				selectedNode->mouseHandler->OnEvent(UIMouseEventHandler::MouseMotionEvent {						
						.node = *selectedNode,
						.timeNS = timeNS,
						.mouseID = 0, //TODO determine mouseID
						.delta = delta,
						.screenPos = screenMousePos,
						.windowPos = windowMousePos,
						.desktopPos = desktopMousePos,
					});
		*/
				
		InputNode* toBeNotifiedSelectedNode = nullptr;		

		if (selectedNode != nullptr && selectedNode->GetMouseMotionReportPolicy() == InputNode::MouseMotionReportPolicy::ReportWhileSelected)
			toBeNotifiedSelectedNode = selectedNode;

		blockingNode = nullptr;

		auto it = screen.GetTree().LastIterator();
		for (;!it.IsNull(); --it)
		{
			if (!it->IsEnabled())
				continue;

			int newHit = it->HitTest(screenMousePos);						

			if (InputNode* inputNode = dynamic_cast<InputNode*>(it.Ptr()))
			{				
				int oldHit = inputNode->mouseHitStatus;
				inputNode->SetMouseHitStatus(newHit);				
				
				if (newHit != 0)
				{
					inputNode->mouseMotionEventDispatcher.Call({
							.node = *inputNode,
							.timeNS = timeNS,
							.mouseID = 0, //TODO determine mouseID
							.delta = delta,
							.screenPos = screenMousePos,
							.windowPos = windowMousePos,
							.desktopPos = desktopMousePos,
						});

					//Set the toBeNotifiedSelectedNode to nullptr because it was already notified of the mouses movement
					if (inputNode == toBeNotifiedSelectedNode)
						toBeNotifiedSelectedNode = nullptr;
				}
			}

			if (newHit > 0)
			{
				blockingNode = it.Ptr();
				--it;
				break;
			}			
		}		

		List<int> a;		

		//Update all the blocked nodes
		for (; !it.IsNull(); --it)
			if (InputNode* inputNode = dynamic_cast<InputNode*>(it.Ptr()))
				inputNode->SetMouseHitStatus(0);			

		if (toBeNotifiedSelectedNode != nullptr && toBeNotifiedSelectedNode == selectedNode)
			toBeNotifiedSelectedNode->mouseMotionEventDispatcher.Call({
					.node = *selectedNode,
					.timeNS = timeNS,
					.mouseID = 0, //TODO determine mouseID
					.delta = delta,
					.screenPos = screenMousePos,
					.windowPos = windowMousePos,
					.desktopPos = desktopMousePos,
				});
	}								
	void InputManager::ScreenWindowChangedEvent(const UI::ScreenWindowChangedEvent& event)
	{
		if (event.oldWindow != nullptr)
			UnsubscribeFromWindow(*event.oldWindow);
		
		if (screen.GetWindow() != nullptr)					
			SubscribeToWindow(*screen.GetWindow());		
	}
	void InputManager::WindowFocusLostEvent(const Window::WindowFocusLostEvent& event)
	{
		SelectNode(nullptr);
	}
	void InputManager::MouseMotionEvent(const Mouse::MouseMotionEvent& event)
	{		
		UpdateScreenMousePos(event.pos);
		windowMousePos = event.pos;
		desktopMousePos = event.desktopPos;

		blockingNode = nullptr;

		CastMousePointer(event.delta, event.timeNS);
	}
	void InputManager::MouseScrollEvent(const Mouse::MouseScrollEvent& event)
	{					
		auto end = --NodeTreeIterator(blockingNode);
		for (auto it = screen.GetTree().LastIterator(); it != end; --it)
		{
			InputNode* node = dynamic_cast<InputNode*>(&*it);

			if (node == nullptr || !node->IsEnabled() || node->mouseHitStatus == 0)
				continue;
			
			node->mouseScrollEventDispatcher.Call({
				.node = *node,
				.timeNS = event.timeNS,
				.mouseID = 0, //TODO determine mouseID
				.value = event.amount,
				.direction = event.direction,
				.screenPos = screenMousePos,
				.windowPos = event.pos,
				.desktopPos = event.desktopPos		
				});

			if (node->mouseHitStatus > 0)
				break;
		}
	}
	void InputManager::MouseButtonDownEvent(const Mouse::MouseButtonDownEvent& event)
	{		
		bool selectedNodeHit = false;
		InputNode* oldSelectedNode = selectedNode;

		auto end = blockingNode == nullptr ? nullptr : blockingNode->GetPreviousNodeInTree();
		for (auto it = screen.GetTree().LastIterator(); it != end; --it)
		{
			InputNode* node = dynamic_cast<InputNode*>(&*it);
			
			if (node == nullptr || !node->IsEnabled() || node->mouseHitStatus == 0)
				continue;
						
			node->mouseButtonDownEventDispatcher.Call({
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

			if (node->mouseHitStatus > 0)
				break;
		}

		if (!selectedNodeHit && oldSelectedNode == selectedNode)
			SelectNode(nullptr);
	}
	void InputManager::MouseButtonUpEvent(const Mouse::MouseButtonUpEvent& event)
	{
		bool selectedNodeAcceptedEvent = false;

		if (selectedNode != nullptr)
		{
			selectedNode->mouseButtonUpEventDispatcher.Call({
				.node = *selectedNode,
				.timeNS = event.timeNS,
				.mouseID = event.mouseID,
				.button = event.button,
				.combo = event.combo,
				.screenPos = screenMousePos,
				.windowPos = event.pos,
				.desktopPos = event.desktopPos,
				});
			
			return;
		}

		auto end = blockingNode == nullptr ? nullptr : blockingNode->GetPreviousNodeInTree();		
		for (auto it = screen.GetTree().LastIterator(); it != end; --it)
		{
			InputNode* node = dynamic_cast<InputNode*>(&*it);

			if (node == nullptr || !node->IsEnabled() || node->mouseHitStatus == 0)
				continue;

			if (node != selectedNode)
				node->mouseButtonUpEventDispatcher.Call({
					.node = *node,
					.timeNS = event.timeNS,
					.mouseID = event.mouseID,
					.button = event.button,
					.combo = event.combo,
					.screenPos = screenMousePos,
					.windowPos = event.pos,
					.desktopPos = event.desktopPos,
					});

			if (node->mouseHitStatus > 0)
				return;
		}
	}
	void InputManager::KeyDownEvent(const Keyboard::KeyDownEvent& event)
	{									
		if (selectedNode == nullptr)
			return;		

		selectedNode->keyDownEventDispatcher.Call({
				.node = *selectedNode,
				.timeNS = event.timeNS,
				.keyboardID = event.keyboardID,
				.key = event.key,
				.modifier = event.modifier,
				.keyCode = event.keyCode,
				.repeat = event.repeat,				
			});		
	}
	void InputManager::KeyUpEvent(const Keyboard::KeyUpEvent& event)
	{
		if (selectedNode == nullptr)
			return;		

		selectedNode->keyUpEventDispatcher.Call({				
				.node = *selectedNode,
				.timeNS = event.timeNS,
				.keyboardID = event.keyboardID,
				.key = event.key,
				.modifier = event.modifier,
				.keyCode = event.keyCode,				
			});
	}
	void InputManager::TextInputEvent(const Keyboard::TextInputEvent& event)
	{		
		if (selectedNode == nullptr)
			return;				

		selectedNode->textInputEventDispatcher.Call({			
			.node = *selectedNode,
			.string = event.string
			});
	}
}
