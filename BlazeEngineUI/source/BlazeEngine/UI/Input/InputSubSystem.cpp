#include "pch.h"
#include "BlazeEngine/UI/Input/InputSubSystem.h"
#include "BlazeEngine/Core/Debug/Logger.h"
#include "BlazeEngine/UI/Input/PointerData.h"

namespace Blaze::UI
{
	InputSubSystem::InputSubSystem()
		: screen(nullptr), selectedNode(nullptr), window(nullptr)
	{

	}
	InputSubSystem::InputSubSystem(Screen& screen)
		: InputSubSystem()
	{
		SetScreen(&screen);
	}
	InputSubSystem::~InputSubSystem()
	{
		SetScreen(nullptr);
	}
	void InputSubSystem::SetScreen(Screen* newScreen)
	{
		if (screen == newScreen)
			return;

		if (screen != nullptr)
		{
			for (auto& node : screen->GetTree())
				if (auto inputNode = dynamic_cast<InputNode*>(&node))
					inputNode->inputSubSystem = nullptr;

			for (auto& node : screen->GetTree())
			{
				node.enabledStateChangedEventDispatcher.RemoveHandler<&InputSubSystem::NodeEnabledStateChangedEvent>(*this);
			}

			screen->destructionEventDispatcher.RemoveHandler<&InputSubSystem::ScreenDestructionEvent>(*this);
			screen->treeChangedEventDispatcher.RemoveHandler<&InputSubSystem::ScreenTreeChangedEvent>(*this);
		}

		pointerMap.Clear();
		screen = newScreen;

		if (screen != nullptr)
		{
			screen->treeChangedEventDispatcher.AddHandler<&InputSubSystem::ScreenTreeChangedEvent>(*this);
			screen->destructionEventDispatcher.AddHandler<&InputSubSystem::ScreenDestructionEvent>(*this);

			for (auto& node : screen->GetTree())
			{
				node.enabledStateChangedEventDispatcher.AddHandler<&InputSubSystem::NodeEnabledStateChangedEvent>(*this);
			}

			for (auto& node : screen->GetTree())
				if (auto inputNode = dynamic_cast<InputNode*>(&node))
					inputNode->inputSubSystem = this;

			if (window != nullptr)
				NewScreenAndWindow();
		}		
	}
	void InputSubSystem::SetWindow(Window* newWindow)
	{
		if (window == newWindow)
			return;

		if (window != nullptr)
		{
			window->destructionEventDispatcher.RemoveHandler<&InputSubSystem::WindowDestructionEvent>(*this);
		}

		pointerMap.Clear();
		window = newWindow;

		if (window != nullptr)
		{
			window->destructionEventDispatcher.AddHandler<&InputSubSystem::WindowDestructionEvent>(*this);

			if (screen != nullptr)
				NewScreenAndWindow();
		}
	}
	void InputSubSystem::SelectNode(InputNode* newSelectedInputNode)
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
			BLAZE_LOG_ERROR("Trying to select a disabled node");
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
	Input::EventProcessedState InputSubSystem::ProcessInputEvent(const Input::GenericInputEvent& event, bool processed)
	{
		Input::EventProcessedState result;

		switch (event.GetValueType())
		{
		case Input::GenericInputEvent::GetValueTypeOf<Input::MouseMotionEvent>():
			event.TryProcess([&](const Input::MouseMotionEvent& e) {
				if (auto it = pointerMap.Find(e.mouseID); !it.IsNull())
					result = it->value.MouseMotionEvent(e, processed);
				});
			break;
		case Input::GenericInputEvent::GetValueTypeOf<Input::MouseScrollEvent>():
			event.TryProcess([&](const Input::MouseScrollEvent& e) {
				if (auto it = pointerMap.Find(e.mouseID); !it.IsNull())
					result = it->value.MouseScrollEvent(e, processed);
				});
			break;
		case Input::GenericInputEvent::GetValueTypeOf<Input::MouseButtonDownEvent>():
			event.TryProcess([&](const Input::MouseButtonDownEvent& e) {
				if (auto it = pointerMap.Find(e.mouseID); !it.IsNull())
				{
					result = it->value.MouseButtonDownEvent(e, processed);

					if (!result)
						SelectNode(nullptr);
				}
				});
			break;
		case Input::GenericInputEvent::GetValueTypeOf<Input::MouseButtonUpEvent>():
			event.TryProcess([&](const Input::MouseButtonUpEvent& e) {
				if (auto it = pointerMap.Find(e.mouseID); !it.IsNull())
					result = it->value.MouseButtonUpEvent(e, processed);
				});
			break;
		case Input::GenericInputEvent::GetValueTypeOf<Input::MouseEntersWindowEvent>():
			event.TryProcess([&](const Input::MouseEntersWindowEvent& e) { result = MouseEntersWindowEvent(e, processed); });
			break;
		case Input::GenericInputEvent::GetValueTypeOf<Input::MouseLeavesWindowEvent>():
			event.TryProcess([&](const Input::MouseLeavesWindowEvent& e) { result = MouseLeavesWindowEvent(e, processed); });
			break;
		case Input::GenericInputEvent::GetValueTypeOf<Input::KeyDownEvent>():
			event.TryProcess([&](const Input::KeyDownEvent& e) { result = KeyDownEvent(e, processed); });
			break;
		case Input::GenericInputEvent::GetValueTypeOf<Input::KeyUpEvent>():
			event.TryProcess([&](const Input::KeyUpEvent& e) { result = KeyUpEvent(e, processed); });
			break;
		case Input::GenericInputEvent::GetValueTypeOf<Input::TextInputEvent>():
			event.TryProcess([&](const Input::TextInputEvent& e) { result = TextInputEvent(e, processed); });
			break;
		}

		return result;
	}
	bool InputSubSystem::SetMouseCapturingNode(Input::MouseID mouseID, InputNode* node)
	{
		if (node != nullptr && !node->IsEnabled())
		{
			BLAZE_LOG_WARNING("A disabled node cannot capture the mouse");
			return false;
		}

		auto it = pointerMap.Find(mouseID);

		if (it.IsNull())
			return false;

		bool returnValue = it->value.SetCapturingNode(node);
		
		Vec2f pos = it->value.GetPos();
		Vec2f windowSize = Vec2f(window->GetSize());
		if (pos.x < 0.0f || pos.y < 0.0f || pos.x >= windowSize.x || pos.y >= windowSize.y)
			pointerMap.Erase(mouseID);

		return returnValue;
	}
	InputNode* InputSubSystem::GetMouseCapturingNode(Input::MouseID mouseID) const
	{
		auto it = pointerMap.Find(mouseID);

		if (it.IsNull())
			return nullptr;

		return it->value.GetCapturingNode();
	}
	Vec2f InputSubSystem::GetScreenMousePos(Input::MouseID mouseID) const
	{
		auto it = pointerMap.Find(mouseID);

		if (it.IsNull())
			return Vec2f();

		return it->value.GetPos();
	}
	void InputSubSystem::NewScreenAndWindow()
	{
		//Rethrow mouse motion event to "hit test" the entire application again
		auto mice = Input::GetMice();

		for (auto& mouse : mice)
			if (mouse.window == window)
			{
				pointerMap.Insert(mouse.id, *screen, *window, mouse.id, mouse.pos, true);

				Input::MouseMotionEvent event{
					.window = *window,
					.timeNS = 0,
					.mouseID = mouse.id,
					.delta = { 0, 0},
					.pos = mouse.pos,
				};

				window->AddInputEvent(event);
			}
	}
	void InputSubSystem::ScreenTreeChangedEvent(const Screen::ScreenTreeChangedEvent& event)
	{
		if (event.type == Screen::ScreenTreeChangedEvent::Type::NodeRemoved)
		{
			if (InputNode* inputNode = dynamic_cast<InputNode*>(&event.node))
			{
				if (selectedNode == inputNode)
					SelectNode(nullptr);

				inputNode->inputSubSystem = nullptr;
			}
		}
	}
	void InputSubSystem::ScreenDestructionEvent(const Screen::DestructionEvent& event)
	{
		SetScreen(nullptr);
	}
	void InputSubSystem::WindowDestructionEvent(const Window::DestructionEvent& event)
	{
		SetWindow(nullptr);
	}
	void InputSubSystem::NodeEnabledStateChangedEvent(const Node::NodeEnabledStateChangedEvent& event)
	{
		if (InputNode* inputNode = dynamic_cast<InputNode*>(&event.node); inputNode != nullptr && selectedNode == inputNode)
				SelectNode(nullptr);
	}
	Input::EventProcessedState InputSubSystem::MouseEntersWindowEvent(const Input::MouseEntersWindowEvent& event, bool processed)
	{
		if (screen == nullptr)
			return Input::EventProcessedState::NotProcessed;

		pointerMap.Insert(event.mouseID, *screen, *window, event.mouseID, event.pos, processed);

		return Input::EventProcessedState::Processed;
	}
	Input::EventProcessedState InputSubSystem::MouseLeavesWindowEvent(const Input::MouseLeavesWindowEvent& event, bool processed)
	{
		auto it = pointerMap.Find(event.mouseID);

		if (it.IsNull())
			return Input::EventProcessedState::NotProcessed;

		if (it->value.GetCapturingNode() == nullptr)
			pointerMap.Erase(it);

		return Input::EventProcessedState::Processed;
	}
	Input::EventProcessedState InputSubSystem::KeyDownEvent(const Input::KeyDownEvent& event, bool processed)
	{
		if (selectedNode == nullptr)
			return Input::EventProcessedState::NotProcessed;

		selectedNode->keyDownEventDispatcher.Call({
			event,
			*selectedNode
			});

		return Input::EventProcessedState::Consumed;
	}
	Input::EventProcessedState InputSubSystem::KeyUpEvent(const Input::KeyUpEvent& event, bool processed)
	{
		if (selectedNode == nullptr)
			return Input::EventProcessedState::NotProcessed;

		selectedNode->keyUpEventDispatcher.Call({
			event,
			*selectedNode
			});

		return Input::EventProcessedState::Consumed;
	}
	Input::EventProcessedState InputSubSystem::TextInputEvent(const Input::TextInputEvent& event, bool processed)
	{
		if (selectedNode == nullptr)
			return Input::EventProcessedState::NotProcessed;

		selectedNode->textInputEventDispatcher.Call({
			event,
			*selectedNode
			});

		return Input::EventProcessedState::Consumed;
	}
}
