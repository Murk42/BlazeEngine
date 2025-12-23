#include "pch.h"
#include "BlazeEngine/UI/Input/InputSubSystem.h"
#include "BlazeEngine/Core/Debug/Logger.h"
#include <ranges>

namespace Blaze::UI
{
	PointerData::PointerData(InputSubSystem& inputSubSystem, Vec2f pos)
		: capturingNode(nullptr), pos(pos), inputSubSystem(inputSubSystem)
	{
		UpdateHitNodes();
	}
	PointerData::~PointerData()
	{
		//Notify all that were hit but aren't no more
		for (auto& hitVal : hitData)
			if (hitVal.hitStatus != Node::HitStatus::NotHit)
				if (InputNode* inputNode = dynamic_cast<InputNode*>(&hitVal.node))
					inputNode->mouseHitStatusChangedEventDispatcher.Call({
						.inputSubSystem = inputSubSystem,
						.oldHitStatus = hitVal.hitStatus,
						.newHitStatus = Node::HitStatus::NotHit
						});
	}
	void PointerData::Move(Vec2f newPos)
	{
		pos = newPos;
		UpdateHitNodes();
	}
	void PointerData::UpdateHitNodes()
	{
		Array<NodeHitData> oldHitData = std::move(hitData);

		if (Screen* screen = inputSubSystem.GetScreen())
		{
			if (capturingNode != nullptr)
			{
				Node::HitStatus newHitStatus = capturingNode->HitTest(pos);

				if (hitData.Count() == 1 && &hitData[0].node == capturingNode)
					hitData[0].hitStatus = newHitStatus;
				else
					hitData = ArrayView<NodeHitData>{ NodeHitData{.node = *capturingNode, .hitStatus = newHitStatus } };
			}
			else
			{

				auto treeView = screen->GetTree();
				for (auto& node : std::ranges::reverse_view(treeView))
				{
					if (!node.IsEnabled())
						continue;

					Node::HitStatus newHit = node.HitTest(pos);

					if (newHit != Node::HitStatus::NotHit)
						hitData.AddBack(node, newHit);

					if (newHit == Node::HitStatus::HitBlocking)
						break;
				}
			}
		}
		else
			hitData.Clear();

		//Notify all that were hit but aren't no more
		for (auto& oldHitVal : oldHitData)
		{
			Node::HitStatus oldHit = oldHitVal.hitStatus;
			Node::HitStatus newHit = Node::HitStatus::NotHit;

			for (auto& newHitVal : hitData)
				if (&newHitVal.node == &oldHitVal.node)
				{
					newHit = newHitVal.hitStatus;
					break;
				}

			if (newHit == Node::HitStatus::NotHit)
				if (InputNode* inputNode = dynamic_cast<InputNode*>(&oldHitVal.node))
					inputNode->mouseHitStatusChangedEventDispatcher.Call({
						.inputSubSystem = inputSubSystem,
						.oldHitStatus = oldHit,
						.newHitStatus = newHit
						});
		}

		//Notify all that were hit and still are but whose hit status changed
		for (auto& newHitVal : hitData)
			for (auto& oldHitVal : oldHitData)
				if (&oldHitVal.node == &newHitVal.node && newHitVal.hitStatus != oldHitVal.hitStatus)
				{
					if (InputNode* inputNode = dynamic_cast<InputNode*>(&oldHitVal.node))
						inputNode->mouseHitStatusChangedEventDispatcher.Call({
							.inputSubSystem = inputSubSystem,
							.oldHitStatus = oldHitVal.hitStatus,
							.newHitStatus = newHitVal.hitStatus
							});
					break;
				}

		//Notify all that weren't hit but are now
		for (auto& newHitVal : hitData)
		{
			Node::HitStatus oldHit = Node::HitStatus::NotHit;
			Node::HitStatus newHit = newHitVal.hitStatus;

			for (auto& oldHitVal : oldHitData)
				if (&oldHitVal.node == &newHitVal.node)
				{
					oldHit = oldHitVal.hitStatus;
					break;
				}

			if (oldHit == Node::HitStatus::NotHit)
				if (InputNode* inputNode = dynamic_cast<InputNode*>(&newHitVal.node))
					inputNode->mouseHitStatusChangedEventDispatcher.Call({
						.inputSubSystem = inputSubSystem,
						.oldHitStatus = oldHit,
						.newHitStatus = newHit
						});
		}
	}
	void PointerData::SetCapturingNode(InputNode* node)
	{
		capturingNode = node;
		UpdateHitNodes();
	}
	void PointerData::HandleNodeRemoval(Node& node)
	{
		InputNode* inputNode = dynamic_cast<InputNode*>(&node);

		if (inputNode != nullptr && capturingNode == inputNode)
			SetCapturingNode(nullptr);
		else
		{
			auto hitDataIt = hitData.FirstIterator();

			for (; hitDataIt != hitData.BehindIterator(); ++hitDataIt)
				if (&hitDataIt->node == &node)
					break;

			if (hitDataIt != hitData.BehindIterator())
				switch (hitDataIt->hitStatus)
				{
				case Node::HitStatus::HitBlocking:
					UpdateHitNodes();
					break;
				case Node::HitStatus::HitNotBlocking:
					hitData.EraseAt(hitDataIt);
					break;
				default:
					BLAZE_LOG_ERROR("Invalid hit status for node in hit data");
					break;
				}
		}
	}
	void PointerData::HandleNodeAddition(Node& node)
	{
		if (node.HitTest(pos) != Node::HitStatus::NotHit)
			UpdateHitNodes();
	}
	void PointerData::HandleEnabledStateChange(Node& node)
	{
		if (node.IsEnabled())
			HandleNodeAddition(node);
		else
			HandleNodeRemoval(node);
	}
	void PointerData::HandleFinalTransformChange(Node& node)
	{
		auto newHitStatus = node.HitTest(pos);
		Node::HitStatus oldHitStatus = Node::HitStatus::NotHit;

		for (auto& oldHitVal : hitData)
			if (&oldHitVal.node == &node)
			{
				oldHitStatus = oldHitVal.hitStatus;
				break;
			}

		if (newHitStatus != oldHitStatus)
			UpdateHitNodes(); //This can be potentially optimized to not update all of the nodes but just one
	}
	//PointerData& PointerData::operator=(const PointerData& other)
	//{
	//	hitData = Array<NodeHitData>(other.hitData);
	//	pos = other.pos;
	//	capturingNode = other.capturingNode;
	//	return *this;
	//}
	//PointerData& PointerData::operator=(PointerData&& other) noexcept
	//{
	//	hitData = std::move(other.hitData);
	//	pos = other.pos;
	//	capturingNode = other.capturingNode;
	//	other.capturingNode = nullptr;
	//
	//	return *this;
	//}

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

			screen->destructionEventDispatcher.RemoveHandler<&InputSubSystem::ScreenDestructionEvent>(*this);
			screen->treeChangedEventDispatcher.RemoveHandler<&InputSubSystem::TreeChangedEvent>(*this);
		}

		screen = newScreen;

		if (screen != nullptr)
		{
			screen->treeChangedEventDispatcher.AddHandler<&InputSubSystem::TreeChangedEvent>(*this);
			screen->destructionEventDispatcher.AddHandler<&InputSubSystem::ScreenDestructionEvent>(*this);

			for (auto& node : screen->GetTree())
				if (auto inputNode = dynamic_cast<InputNode*>(&node))
					inputNode->inputSubSystem = this;
		}

		for (auto& pair : pointerMap)
			pair.value.UpdateHitNodes();
	}
	void InputSubSystem::SetWindow(Window* newWindow)
	{
		if (window == newWindow)
			return;

		if (window != nullptr)
		{
			window->destructionEventDispatcher.RemoveHandler<&InputSubSystem::WindowDestructionEvent>(*this);
			window->mouseEnterEventDispatcher.RemoveHandler<&InputSubSystem::WindowMouseEnterEvent>(*this);
			window->mouseLeaveEventDispatcher.RemoveHandler<&InputSubSystem::WindowMouseLeaveEvent>(*this);
		}

		window = newWindow;

		if (window != nullptr)
		{
			window->mouseLeaveEventDispatcher.AddHandler<&InputSubSystem::WindowMouseLeaveEvent>(*this);
			window->mouseEnterEventDispatcher.AddHandler<&InputSubSystem::WindowMouseEnterEvent>(*this);
			window->destructionEventDispatcher.AddHandler<&InputSubSystem::WindowDestructionEvent>(*this);
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
	void InputSubSystem::InvalidateNodeHitStatus(InputNode& node)
	{
		for (auto& pair : pointerMap)
			pair.value.UpdateHitNodes();
	}
	bool InputSubSystem::ProcessInputEvent(const Input::GenericInputEvent& event, bool eventProcessed)
	{
		switch (event.GetValueType())
		{
		case Input::GenericInputEvent::GetValueTypeOf<Input::MouseMotionEvent>():
			event.TryProcess<Input::MouseMotionEvent>([&](const auto& e) { eventProcessed = MouseMotionEvent(e, eventProcessed); });
			break;
		case Input::GenericInputEvent::GetValueTypeOf<Input::MouseScrollEvent>():
			event.TryProcess<Input::MouseScrollEvent>([&](const auto& e) { eventProcessed = MouseScrollEvent(e, eventProcessed); });
			break;
		case Input::GenericInputEvent::GetValueTypeOf<Input::MouseButtonDownEvent>():
			event.TryProcess<Input::MouseButtonDownEvent>([&](const auto& e) { eventProcessed = MouseButtonDownEvent(e, eventProcessed); });
			break;
		case Input::GenericInputEvent::GetValueTypeOf<Input::MouseButtonUpEvent>():
			event.TryProcess<Input::MouseButtonUpEvent>([&](const auto& e) { eventProcessed = MouseButtonUpEvent(e, eventProcessed); });
			break;
		case Input::GenericInputEvent::GetValueTypeOf<Input::KeyDownEvent>():
			event.TryProcess<Input::KeyDownEvent>([&](const auto& e) { eventProcessed = KeyDownEvent(e, eventProcessed); });
			break;
		case Input::GenericInputEvent::GetValueTypeOf<Input::KeyUpEvent>():
			event.TryProcess<Input::KeyUpEvent>([&](const auto& e) { eventProcessed = KeyUpEvent(e, eventProcessed); });
			break;
		case Input::GenericInputEvent::GetValueTypeOf<Input::TextInputEvent>():
			event.TryProcess<Input::TextInputEvent>([&](const auto& e) { eventProcessed = TextInputEvent(e, eventProcessed); });
			break;
		}

		return eventProcessed;
	}
	bool InputSubSystem::SetMouseCapturingNode(Input::MouseID mouseID, InputNode* node)
	{
		if (node != nullptr && !node->IsEnabled())
		{
			BLAZE_LOG_WARNING("A disabled node cannot capture the mouse");
			return false;
		}

		if (window != nullptr)
		{
			if (node == nullptr)
			{
				mouseCaptureHandle.ReleaseMouseCapture();
			}
			else
			{
				if (!window->CaptureMouse(mouseCaptureHandle))
					return false;
			}
		}

		auto pointerData = FindPointerData(mouseID);

		if (pointerData == nullptr)
			return false;

		pointerData->SetCapturingNode(node);

		return true;
	}
	InputNode* InputSubSystem::GetMouseCapturingNode(Input::MouseID mouseID) const
	{
		auto pointerData = FindPointerData(mouseID);

		if (pointerData == nullptr)
			return nullptr;

		return pointerData->GetCapturingNode();
	}
	Vec2f InputSubSystem::GetScreenMousePos(Input::MouseID mouseID) const
	{
		auto pointerData = FindPointerData(mouseID);

		if (pointerData == nullptr)
			return Vec2f();

		return pointerData->GetPos();
	}
	void InputSubSystem::TreeChangedEvent(const Screen::TreeChangedEvent& event)
	{
		if (event.type == Screen::TreeChangedEvent::Type::NodeRemoved)
		{
			if (InputNode* inputNode = dynamic_cast<InputNode*>(&event.node))
			{
				if (selectedNode == inputNode)
					SelectNode(nullptr);

				inputNode->inputSubSystem = nullptr;
			}

			for (auto& pair : pointerMap)
				pair.value.HandleNodeRemoval(event.node);
		}
		else if (event.type == Screen::TreeChangedEvent::Type::NodeAdded)
		{
			for (auto& pair : pointerMap)
				pair.value.HandleNodeAddition(event.node);
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
	void InputSubSystem::WindowMouseEnterEvent(const Window::MouseEnterEvent& event)
	{
	}
	void InputSubSystem::WindowMouseLeaveEvent(const Window::MouseLeaveEvent& event)
	{
		if (!mouseCaptureHandle.IsCapturingMouse())
			pointerMap.Clear();
	}
	void InputSubSystem::NodeFinalTransformChangedEvent(const Node::FinalTransformUpdatedEvent& event)
	{
		for (auto& pair : pointerMap)
			pair.value.HandleFinalTransformChange(event.node);
	}
	void InputSubSystem::EnabledStateChangedEvent(const Node::EnabledStateChangedEvent& event)
	{
		for (auto& pair : pointerMap)
			pair.value.HandleEnabledStateChange(event.node);
	}
	bool InputSubSystem::MouseMotionEvent(const Input::MouseMotionEvent& event, bool eventProcessed)
	{
		const Vec2u windowSize = event.window.GetSize();
		const Vec2f newMouseWindowPos = Vec2f(event.pos.x, windowSize.y - event.pos.y);		

		const auto screenFinalTransform = screen->GetFinalTransform();

		const Vec2f newMouseScreenPos = screenFinalTransform.TransformFromFinalToLocalTransformSpace(newMouseWindowPos) * screenFinalTransform.size / (Vec2f)windowSize;

		PointerData* pointerData = PrepareMouseEvent(event.mouseID, newMouseScreenPos, eventProcessed, true);

		if (pointerData == nullptr)
			return false;

		const Vec2f oldMouseScreenPos = pointerData->GetPos();
		const Vec2f mouseScreenDelta = newMouseWindowPos - oldMouseScreenPos;

		if (mouseScreenDelta == Vec2f(0, 0))
			return false;

		UIMouseMotionEvent eventUI{ event, *this };
		eventUI.delta = mouseScreenDelta;
		eventUI.pos = newMouseScreenPos;

		pointerData->Move(newMouseScreenPos);

		if (pointerData->GetCapturingNode() != nullptr)
		{
			pointerData->GetCapturingNode()->mouseMotionEventDispatcher.Call(eventUI);
			return true;
		}
		else if (!pointerData->GetHitData().Empty())
		{
			Array<PointerData::NodeHitData> hitData = pointerData->GetHitData(); //Copy hit data
			bool blockingHit = false;
			for (auto& hitData : std::ranges::reverse_view(hitData))
			{
				if (InputNode* inputNode = dynamic_cast<InputNode*>(&hitData.node))
					inputNode->mouseMotionEventDispatcher.Call(eventUI);

				blockingHit |= hitData.hitStatus == Node::HitStatus::HitBlocking;
			}
			return blockingHit;
		}

		return false;
	}
	bool InputSubSystem::MouseScrollEvent(const Input::MouseScrollEvent& event, bool eventProcessed)
	{
		PointerData* pointerData = PrepareMouseEvent(event.mouseID, event.pos, eventProcessed, false);

		if (pointerData == nullptr)
			return false;

		UIMouseScrollEvent eventUI{ event, *this };
		eventUI.pos = pointerData->GetPos();

		if (pointerData->GetCapturingNode() != nullptr)
		{
			pointerData->GetCapturingNode()->mouseScrollEventDispatcher.Call(eventUI);
			return true;
		}
		else if (!pointerData->GetHitData().Empty())
		{
			Array<PointerData::NodeHitData> hitData = pointerData->GetHitData(); //Copy hit data
			bool blockingHit = false;
			for (auto& hitData : std::ranges::reverse_view(hitData))
			{
				if (InputNode* inputNode = dynamic_cast<InputNode*>(&hitData.node))
					inputNode->mouseScrollEventDispatcher.Call(eventUI);

				blockingHit |= hitData.hitStatus == Node::HitStatus::HitBlocking;
			}
			return blockingHit;
		}

		return false;
	}
	bool InputSubSystem::MouseButtonDownEvent(const Input::MouseButtonDownEvent& event, bool eventProcessed)
	{
		PointerData* pointerData = PrepareMouseEvent(event.mouseID, event.pos, eventProcessed, false);

		if (pointerData == nullptr)
			return false;

		UIMouseButtonDownEvent eventUI{ event, *this };
		eventUI.pos = pointerData->GetPos();

		if (pointerData->GetCapturingNode() != nullptr)
		{
			pointerData->GetCapturingNode()->mouseButtonDownEventDispatcher.Call(eventUI);
			return true;
		}
		else if (!pointerData->GetHitData().Empty())
		{
			Array<PointerData::NodeHitData> hitData = pointerData->GetHitData(); //Copy hit data
			bool blockingHit = false;
			for (auto& hitData : std::ranges::reverse_view(hitData))
			{
				if (InputNode* inputNode = dynamic_cast<InputNode*>(&hitData.node))
					inputNode->mouseButtonDownEventDispatcher.Call(eventUI);

				blockingHit |= (hitData.hitStatus == Node::HitStatus::HitBlocking);
			}
			return blockingHit;
		}

		return false;
	}
	bool InputSubSystem::MouseButtonUpEvent(const Input::MouseButtonUpEvent& event, bool eventProcessed)
	{
		PointerData* pointerData = PrepareMouseEvent(event.mouseID, event.pos, eventProcessed, false);

		if (pointerData == nullptr)
			return false;

		UIMouseButtonUpEvent eventUI{ event, *this };
		eventUI.pos = pointerData->GetPos();

		if (pointerData->GetCapturingNode() != nullptr)
		{
			pointerData->GetCapturingNode()->mouseButtonUpEventDispatcher.Call(eventUI);
			return true;
		}
		else if (!pointerData->GetHitData().Empty())
		{
			Array<PointerData::NodeHitData> hitData = pointerData->GetHitData(); //Copy hit data
			bool blockingHit = false;
			for (auto& hitData : std::ranges::reverse_view(hitData))
			{
				if (InputNode* inputNode = dynamic_cast<InputNode*>(&hitData.node))
					inputNode->mouseButtonUpEventDispatcher.Call(eventUI);

				blockingHit |= hitData.hitStatus == Node::HitStatus::HitBlocking;
			}
			return blockingHit;
		}

		return false;
	}
	bool InputSubSystem::KeyDownEvent(const Input::KeyDownEvent& event, bool eventProcessed)
	{
		if (selectedNode == nullptr)
			return false;

		selectedNode->keyDownEventDispatcher.Call({
			event,
			*this
			});

		return true;
	}
	bool InputSubSystem::KeyUpEvent(const Input::KeyUpEvent& event, bool eventProcessed)
	{
		if (selectedNode == nullptr)
			return false;

		selectedNode->keyUpEventDispatcher.Call({
			event,
			* this
			});

		return true;
	}
	bool InputSubSystem::TextInputEvent(const Input::TextInputEvent& event, bool eventProcessed)
	{
		if (selectedNode == nullptr)
			return false;

		selectedNode->textInputEventDispatcher.Call({
			event,
			* this
			});

		return true;
	}
	PointerData* InputSubSystem::FindOrInsertPointerData(Input::MouseID mouseID, Vec2f pos)
	{
		auto it = pointerMap.Insert(mouseID, *this, pos).iterator;
		if (it.IsNull())
		{
			BLAZE_LOG_FATAL("Failed to insert pointer data");
			return nullptr;
		}

		return &it->value;
	}
	const PointerData* InputSubSystem::FindPointerData(Input::MouseID mouseID) const
	{
		auto it = pointerMap.Find(mouseID);

		if (it.IsNull())
			return nullptr;

		return &it->value;
	}
	PointerData* InputSubSystem::FindPointerData(Input::MouseID mouseID)
	{
		auto it = pointerMap.Find(mouseID);

		if (it.IsNull())
			return nullptr;

		return &it->value;
	}
	PointerData* InputSubSystem::PrepareMouseEvent(Input::MouseID mouseID, Vec2f pos, bool eventProcessed, bool insertMouse)
	{
		if (screen == nullptr)
			return nullptr;

		if (window != nullptr && window->IsMouseCaptured() && !mouseCaptureHandle.IsCapturingMouse())
			return nullptr;

		PointerData* pointerData = nullptr;
		
		if (insertMouse)
			pointerData = FindOrInsertPointerData(mouseID, pos);
		else
			pointerData = FindPointerData(mouseID);

		if (pointerData == nullptr)
			return nullptr;

		if (eventProcessed && pointerData->GetCapturingNode() == nullptr)
		{
			pointerMap.Clear();
			return nullptr;
		}

		return pointerData;
	}
}
