#include "pch.h"
#include "BlazeEngine/UI/Input/PointerData.h"
#include "BlazeEngine/UI/Core/Screen.h"
#include "BlazeEngine/Runtime/IO/Window.h"

#include <ranges>

namespace Blaze::UI
{	
	static Vec2f TransformWindowPointToScreenPoint(Vec2f point, Screen& screen, Window& window)
	{
		const Vec2u windowSize = window.GetSize();
		const Vec2f newMouseWindowPos = Vec2f(point.x, windowSize.y - point.y);

		const auto screenFinalTransform = screen.GetFinalTransform();

		return screenFinalTransform.TransformFromFinalToLocalTransformSpace(newMouseWindowPos * screenFinalTransform.scale * screenFinalTransform.size / (Vec2f)windowSize);
	}
	static Vec2f TransformWindowDeltaToScreenDelta(Vec2f delta, Screen& screen, Window& window)
	{
		const Vec2u windowSize = window.GetSize();
		const auto screenFinalTransform = screen.GetFinalTransform();
		return Vec2f(delta.x, -delta.y) * screenFinalTransform.scale * screenFinalTransform.size / (Vec2f)windowSize;
	}

	PointerData::PointerData(Screen& screen, Window& window, Input::MouseID mouseID, Vec2f windowPos, bool blocked) :
		screen(screen), window(window), mouseID(mouseID),
		pos(TransformWindowPointToScreenPoint(windowPos, screen, window)), windowPos(windowPos),
		capturingNode(nullptr), blocked(blocked)
	{
		EventHandlerData<WindowMouseCaptureHandle::CapturedMouseEvent> eventHandlerData;
		eventHandlerData.Set<&PointerData::CapturedMouseEvent>(*this);
		mouseCaptureHandle.SetEventHandlerData(eventHandlerData);

		screen.treeChangedEventDispatcher.AddHandler<&PointerData::ScreenTreeChangedEvent>(*this);
		for (auto& node : screen.GetTree())
			SubscribeToNode(node);

		UpdateHitNodes();
	}
	PointerData::~PointerData()
	{
		if (capturingNode != nullptr)
		{
			mouseCaptureHandle.ReleaseMouseCapture();
			capturingNode = nullptr;
		}

		blocked = true; //Set blocked to true to simulate the mouse not hitting any nodes and therefore notifyting any previous hit nodes before destroying the pointer object
		UpdateHitNodes();

		for (auto& node : screen.GetTree())
			UnsubscribeFromNode(node);
		screen.treeChangedEventDispatcher.RemoveHandler<&PointerData::ScreenTreeChangedEvent>(*this);
	}
	bool PointerData::SetCapturingNode(InputNode* node)
	{
		capturingNode = node;

		if (node == nullptr)
		{
			if (!mouseCaptureHandle.ReleaseMouseCapture())
				return false;
		}
		else
		{
			if (!window.CaptureMouse(mouseID, mouseCaptureHandle))
				return false;
		}

		UpdateSceneMouseCollision();

		return true;
	}
	Input::EventProcessedState PointerData::MouseMotionEvent(const Input::MouseMotionEvent& windowEvent, bool processed)
	{
		const Vec2f newPos = TransformWindowPointToScreenPoint(windowEvent.pos, screen, windowEvent.window);
		const Vec2f delta = TransformWindowDeltaToScreenDelta(windowEvent.delta, screen, windowEvent.window);

		const bool mouseCapturedByOther = capturingNode == nullptr && window.IsMouseCaptured(mouseID);
		//If the event was already processed or the mouse is captured not by this input sub system the pointer 
		const bool newBlocked = processed || mouseCapturedByOther;

		windowPos = windowEvent.pos;

		if (!syntheticMouseMoveEventSent && capturingNode == nullptr && windowEvent.delta == Vec2f(0, 0) && blocked == newBlocked)
			return hitData.Empty() || hitData.Last().hitStatus != Node::HitStatus::HitBlocking ? Input::EventProcessedState::NotProcessed : Input::EventProcessedState::Processed;
		
		syntheticMouseMoveEventSent = false;

		blocked = newBlocked;
		pos = newPos;

		UpdateHitNodes();

		DispatchEvent([&](InputNode& node) {
			UIMouseMotionEvent screenEvent{ windowEvent, node };
			screenEvent.delta = delta;
			screenEvent.pos = newPos;
			node.mouseMotionEventDispatcher.Call(screenEvent);
			});

		return hitData.Empty() || hitData.Last().hitStatus != Node::HitStatus::HitBlocking ? Input::EventProcessedState::NotProcessed : Input::EventProcessedState::Processed;
	}
	Input::EventProcessedState PointerData::MouseScrollEvent(const Input::MouseScrollEvent& windowEvent, bool processed)
	{
		DispatchEvent([&](InputNode& node) {
			UIMouseScrollEvent screenEvent{ windowEvent, node };
			screenEvent.pos = pos;
			node.mouseScrollEventDispatcher.Call(screenEvent);
			});

		return hitData.Empty() ? Input::EventProcessedState::NotProcessed : Input::EventProcessedState::Processed;
	}
	Input::EventProcessedState PointerData::MouseButtonDownEvent(const Input::MouseButtonDownEvent& windowEvent, bool processed)
	{
		bool consumed = DispatchEvent([&](InputNode& node) {
			UIMouseButtonDownEvent screenEvent{ windowEvent, node };
			screenEvent.pos = pos;
			node.mouseButtonDownEventDispatcher.Call(screenEvent);
			});

		return consumed ? Input::EventProcessedState::Consumed : Input::EventProcessedState::NotProcessed;
	}
	Input::EventProcessedState PointerData::MouseButtonUpEvent(const Input::MouseButtonUpEvent& windowEvent, bool processed)
	{
		bool consumed = DispatchEvent([&](InputNode& node) {
			UIMouseButtonUpEvent screenEvent{ windowEvent, node };
			screenEvent.pos = pos;
			node.mouseButtonUpEventDispatcher.Call(screenEvent);
			});

		return consumed ? Input::EventProcessedState::Consumed : Input::EventProcessedState::NotProcessed;
	}
	void PointerData::ScreenTreeChangedEvent(const Screen::ScreenTreeChangedEvent& event)
	{
		if (event.type == Screen::ScreenTreeChangedEvent::Type::NodeRemoved)
		{
			InputNode* inputNode = dynamic_cast<InputNode*>(&event.node);

			if (inputNode != nullptr && capturingNode == inputNode)
				SetCapturingNode(nullptr);
			else
				UpdateNodeHitStatus(event.node, true);

			UnsubscribeFromNode(event.node);
		}
		else if (event.type == Screen::ScreenTreeChangedEvent::Type::NodeAdded)
		{
			SubscribeToNode(event.node);

			UpdateNodeHitStatus(event.node);
		}
	}
	void PointerData::NodeFinalTransformChangedEvent(const Node::FinalTransformUpdatedEvent& event)
	{
		UpdateNodeHitStatus(event.node);
	}
	void PointerData::NodeEnabledStateChangedEvent(const Node::NodeEnabledStateChangedEvent& event)
	{
		UpdateNodeHitStatus(event.node, !event.node.IsEnabled());
	}
	void PointerData::NodeHitTestInvalidatedEvent(const Node::HitTestInvalidatedEvent& event)
	{
		UpdateNodeHitStatus(event.node);
	}
	void PointerData::SubscribeToNode(Node& node)
	{
		node.finalTransformUpdatedEventDispatcher.AddHandler<&PointerData::NodeFinalTransformChangedEvent>(*this);
		node.enabledStateChangedEventDispatcher.AddHandler<&PointerData::NodeEnabledStateChangedEvent>(*this);
		node.hitTestInvalidatedEventDispatcher.AddHandler<&PointerData::NodeHitTestInvalidatedEvent>(*this);
	}
	void PointerData::UnsubscribeFromNode(Node& node)
	{
		node.finalTransformUpdatedEventDispatcher.RemoveHandler<&PointerData::NodeFinalTransformChangedEvent>(*this);
		node.enabledStateChangedEventDispatcher.RemoveHandler<&PointerData::NodeEnabledStateChangedEvent>(*this);
		node.hitTestInvalidatedEventDispatcher.RemoveHandler<&PointerData::NodeHitTestInvalidatedEvent>(*this);
	}
	void PointerData::CapturedMouseEvent(const WindowMouseCaptureHandle::CapturedMouseEvent& event)
	{
		bool eventProcessed = false;

		switch (event.GetValueType())
		{
		case WindowMouseCaptureHandle::CapturedMouseEvent::GetValueTypeOf<Input::MouseScrollEvent>():
			event.TryProcess([&](const Input::MouseScrollEvent& e) { MouseScrollEvent(e, false); });
			break;
		case WindowMouseCaptureHandle::CapturedMouseEvent::GetValueTypeOf<Input::MouseButtonDownEvent>():
			event.TryProcess([&](const Input::MouseButtonDownEvent& e) { MouseButtonDownEvent(e, false); });
			break;
		case WindowMouseCaptureHandle::CapturedMouseEvent::GetValueTypeOf<Input::MouseButtonUpEvent>():
			event.TryProcess([&](const Input::MouseButtonUpEvent& e) { MouseButtonUpEvent(e, false); });
			break;
		}
	}
	void PointerData::UpdateSceneMouseCollision()
	{
		syntheticMouseMoveEventSent = true;
		//By sending a synthetic mouse motion event the pointer is tested for hit testing
		window.AddInputEvent(Input::MouseMotionEvent{ .window = window, .timeNS = 0, .mouseID = mouseID, .delta = { }, .pos = windowPos });
	}
	void PointerData::UpdateNodeHitStatus(Node& node, bool useNotHitAsHitStatus)
	{
		Node::HitStatus newHitStatus = useNotHitAsHitStatus ? Node::HitStatus::NotHit : node.HitTest(pos);
		Node::HitStatus oldHitStatus = Node::HitStatus::NotHit;

		for (auto& oldHitVal : hitData)
			if (&oldHitVal.node == &node)
			{
				oldHitStatus = oldHitVal.hitStatus;
				break;
			}

		if (newHitStatus != oldHitStatus)
		{
			if (newHitStatus == Node::HitStatus::HitBlocking || oldHitStatus == Node::HitStatus::HitBlocking)
			{
				//If the node started or stopped blocking the pointer the whole scene needs to be updated
				UpdateSceneMouseCollision();
			}
			else
			{
				//If the node isn't blocking the pointer it's enough to update the local hit test
				UpdateHitNodes();
			}
		}
	}
	void PointerData::UpdateHitNodes()
	{
		Array<NodeHitData> oldHitData = std::move(hitData);

		Vec2f windowSize = Vec2f(window.GetSize());
		bool isInsideWindow = windowPos.x >= 0 && windowPos.y >= 0 && windowPos.x < windowSize.x && windowPos.y < windowSize.y;

		if (capturingNode != nullptr)
		{
			Node::HitStatus newHitStatus = Node::HitStatus::NotHit;
						
			if (!blocked && isInsideWindow)
			{
				bool nodeHit = false;
				auto treeView = screen.GetTree();
				for (auto& node : std::ranges::reverse_view(treeView))
				{
					if (&node == capturingNode)
						break;

					if (!node.IsEnabled())
						continue;


					Node::HitStatus newHit = node.HitTest(pos);

					if (newHit == Node::HitStatus::HitBlocking)
					{
						nodeHit = true;
						break;
					}
				}

				if (!nodeHit)
					newHitStatus = capturingNode->HitTest(pos);
			}

			if (hitData.Count() == 1 && &hitData[0].node == capturingNode)
				hitData[0].hitStatus = newHitStatus;
			else
				hitData = ArrayView<NodeHitData>{ NodeHitData{.node = *capturingNode, .hitStatus = newHitStatus } };
		}
		else if (!blocked && isInsideWindow)
		{
			auto treeView = screen.GetTree();
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
						.node = *inputNode,
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
							.node = *inputNode,
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
						.node = *inputNode,
						.oldHitStatus = oldHit,
						.newHitStatus = newHit
						});
		}
	}

	template<typename F> requires std::invocable<F, InputNode&>
	inline bool PointerData::DispatchEvent(F function)
	{
		if (capturingNode != nullptr)
		{
			function(*capturingNode);
			return true;
		}
		else if (!hitData.Empty())
		{
			Array<PointerData::NodeHitData> hitDataCopy = hitData; //Copy hit data
			bool blockingHit = false;
			for (auto& hitData : std::ranges::reverse_view(hitDataCopy))
			{
				if (InputNode* inputNode = dynamic_cast<InputNode*>(&hitData.node))
					function(*inputNode);

				blockingHit |= (hitData.hitStatus == Node::HitStatus::HitBlocking);
			}

			return blockingHit;
		}
		else
			return false;
	}
}