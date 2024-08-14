#include "pch.h"
#include "BlazeEngineGraphics/UI/Input/UIInputManager.h"
#include "BlazeEngineGraphics/UI/Input/UIMouseEventHandler.h"
#include "BlazeEngineGraphics/UI/Input/UIKeyboardEventHandler.h"
#include "BlazeEngineGraphics/UI/Input/UISelectEventHandler.h"
#include "BlazeEngine/Input/Input.h"

namespace Blaze::UI
{
	UIInputManager::UIInputManager()
		: selectedNode(nullptr)
	{		
		Input::GetInputEventSystem().inputPreUpdateDispatcher.AddHandler(*this);
		Input::GetInputEventSystem().inputPostUpdateDispatcher.AddHandler(*this);
	}
	UIInputManager::~UIInputManager()
	{		
	}
	void UIInputManager::AddScreen(Screen* screen)
	{
		if (screen != nullptr)
		{
			auto res = screenData.Insert(screen);

			if (!res.inserted)
				return;

			auto& data = res.iterator->value;			

			screensToRecreate.AddBack(screen);

			screen->nodeCreatedEventDispatcher.AddHandler(*this);
			screen->nodeDestroyedEventDispatcher.AddHandler(*this);
			screen->screenDestructionEventDispatcher.AddHandler(*this);

			if (screen->GetWindow() != nullptr)
			{
				screen->GetWindow()->mouseMotionDispatcher.AddHandler(*this);
				screen->GetWindow()->mouseScrollDispatcher.AddHandler(*this);
				screen->GetWindow()->keyPressedDispatcher.AddHandler(*this);
				screen->GetWindow()->keyReleasedDispatcher.AddHandler(*this);
				screen->GetWindow()->textInputDispatcher.AddHandler(*this);
			}
		}
	}

	void UIInputManager::RemoveScreen(Screen* screen)
	{		
		auto it = screenData.Find(screen);
		if (it.IsNull())
			return;

		screenData.Erase(it);
	
		screen->nodeCreatedEventDispatcher.RemoveHandler(*this);
		screen->nodeDestroyedEventDispatcher.RemoveHandler(*this);
		screen->screenDestructionEventDispatcher.RemoveHandler(*this);

		if (screen->GetWindow() != nullptr)
		{
			screen->GetWindow()->mouseMotionDispatcher.RemoveHandler(*this);
			screen->GetWindow()->mouseScrollDispatcher.RemoveHandler(*this);
			screen->GetWindow()->keyPressedDispatcher.RemoveHandler(*this);
			screen->GetWindow()->keyReleasedDispatcher.RemoveHandler(*this);
			screen->GetWindow()->textInputDispatcher.RemoveHandler(*this);
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

	void UIInputManager::RecreateScreenInputNodes(Map<Screen*, ScreenData>::Iterator it)
	{
		it->value.nodes.Clear();

		for (auto child : it->key->GetChildren())
			GatherInputNodes(child, it->value.nodes);		

		for (auto node : it->value.nodes)
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
	}
	void UIInputManager::CheckMousePositionEvents(Vec2i delta)
	{
		Vec2i desktopMousePos = Input::GetDesktopMousePos();

		auto oldSelectedNode = selectedNode;
	
		for (auto& data : screenData)
		{
			if (data.key->GetWindow() == nullptr)
				continue;

			Vec2f mousePos = Vec2f(desktopMousePos - data.key->GetWindow()->GetPos());

			bool alreadyHit = false;
			for (auto node : data.value.nodes)
			{
				bool oldHit = node->hit;
				bool newHit = node->HitTest(mousePos) && !alreadyHit;
				node->hit = newHit;

				if (auto mouseEventHandler = node->mouseHandler)
				{
					if (newHit && !oldHit)
						mouseEventHandler->OnEvent(UIMouseEventHandler::MouseEnterEvent({
							.inputManager = this,
							.pos = mousePos
							}));

					if (oldHit && !newHit)
						mouseEventHandler->OnEvent(UIMouseEventHandler::MouseExitEvent({
							.inputManager = this,
							.pos = mousePos
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
	}		
	void UIInputManager::OnEvent(NodeCreatedEvent event)
	{
		if (auto inputNode = dynamic_cast<UIInputNode*>(event.node))
		{			
			auto it = screenData.Find(event.node->GetScreen());

			if (it.IsNull())
			{
				Debug::Logger::LogWarning("Blaze Engine Graphics", "Couldn't find screen");
				return;
			}

			screensToRecreate.AddBack(it->key);
		}
	}
	void UIInputManager::OnEvent(NodeDestroyedEvent event)
	{
		if (auto inputNode = dynamic_cast<UIInputNode*>(event.node))
		{
			auto it = screenData.Find(event.node->GetScreen());

			if (it.IsNull())
			{
				Debug::Logger::LogWarning("Blaze Engine Graphics", "Couldn't find screen");
				return;
			}

			screensToRecreate.AddBack(it->key);
		}
	}
	void UIInputManager::OnEvent(ScreenDestructionEvent event)
	{
		RemoveScreen(event.screen);
	}
	void UIInputManager::OnEvent(ScreenWindowChangedEvent event)
	{
		if (event.oldWindow != nullptr)
		{
			event.oldWindow->mouseMotionDispatcher.RemoveHandler(*this);
			event.oldWindow->mouseScrollDispatcher.RemoveHandler(*this);
			event.oldWindow->keyPressedDispatcher.RemoveHandler(*this);
			event.oldWindow->keyReleasedDispatcher.RemoveHandler(*this);
			event.oldWindow->textInputDispatcher.RemoveHandler(*this);
		}

		if (event.screen->GetWindow() != nullptr)
		{
			event.screen->GetWindow()->mouseMotionDispatcher.AddHandler(*this);
			event.screen->GetWindow()->mouseScrollDispatcher.AddHandler(*this);
			event.screen->GetWindow()->keyPressedDispatcher.AddHandler(*this);
			event.screen->GetWindow()->keyReleasedDispatcher.AddHandler(*this);
			event.screen->GetWindow()->textInputDispatcher.AddHandler(*this);
		}
	}
	void UIInputManager::OnEvent(Input::Events::InputPreUpdateEvent event)
	{						
		for (auto& value : screensToRecreate)
		{
			auto it = screenData.Find(value);

			if (it.IsNull())
				continue;

			RecreateScreenInputNodes(it);
		}

		screensToRecreate.Clear();
	}
	void UIInputManager::OnEvent(Input::Events::InputPostUpdateEvent event)
	{
		CheckMousePositionEvents(Vec2i());
	}
	void UIInputManager::OnEvent(Input::Events::MouseMotion event)
	{				
		CheckMousePositionEvents(event.delta);		
	}
	void UIInputManager::OnEvent(Input::Events::MouseScroll event)
	{
		Vec2i desktopMousePos = Input::GetDesktopMousePos();
		for (auto& data : screenData)
		{
			if (data.key->GetWindow() == nullptr)
				continue;

			Vec2f mousePos = Vec2f(desktopMousePos - data.key->GetWindow()->GetPos());

			for (auto node : data.value.nodes)
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
	}
	void UIInputManager::OnEvent(Input::Events::KeyPressed event)
	{
		UIMouseEventHandler::MouseButton button = (UIMouseEventHandler::MouseButton)((uint)event.key - (uint)Key::MouseLeft);			
		
		if ((uint)button < 3)
		{
			bool selectedNodeHit = false;
			auto oldSelectedNode = selectedNode;

			Vec2i desktopMousePos = Input::GetDesktopMousePos();

			for (auto& data : screenData)
			{
				if (data.key->GetWindow() == nullptr)
					continue;

				Vec2f mousePos = Vec2f(desktopMousePos - data.key->GetWindow()->GetPos());

				for (auto node : data.value.nodes)
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
			Vec2i desktopMousePos = Input::GetDesktopMousePos();

			if (selectedNode != nullptr && selectedNode->mouseHandler != nullptr)
			{
				Vec2f mousePos = Vec2f(desktopMousePos - selectedNode->GetScreen()->GetWindow()->GetPos());

				selectedNode->mouseHandler->OnEvent(UIMouseEventHandler::MouseReleasedEvent({
					.inputManager = this,
					.pos = mousePos,
					.button = button,
					.time = event.time
					}));
			}


			for (auto& data : screenData)
			{
				if (data.key->GetWindow() == nullptr)
					continue;

				Vec2f mousePos = Vec2f(desktopMousePos - data.key->GetWindow()->GetPos());

				for (auto node : data.value.nodes)
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
