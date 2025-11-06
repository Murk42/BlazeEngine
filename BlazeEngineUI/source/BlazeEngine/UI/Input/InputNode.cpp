#include "pch.h"
#include "BlazeEngine/UI/Input/InputManager.h"
#include "BlazeEngine/UI/Input/InputNode.h"

namespace Blaze::UI
{
	InputNode::InputNode()
		: Node(), mouseHitStatus(0), inputManager(nullptr), selectable(true), mouseMotionReportPolicy(MouseMotionReportPolicy::ReportWhileMouseInside)
	{
		finalTransformUpdatedEventDispatcher.AddHandler<&InputNode::FinalTransformUpdatedEvent>(*this);
		enabledStateChangedEventDispatcher.AddHandler<&InputNode::EnabledStateChangedEvent>(*this);
		surroundingNodeTreeChangedEventDispatcher.AddHandler<&InputNode::SurroundingNodeTreeChangedEvent>(*this);
	}
	InputNode::~InputNode()
	{
		surroundingNodeTreeChangedEventDispatcher.RemoveHandler<&InputNode::SurroundingNodeTreeChangedEvent>(*this);
		enabledStateChangedEventDispatcher.RemoveHandler<&InputNode::EnabledStateChangedEvent>(*this);
		finalTransformUpdatedEventDispatcher.RemoveHandler<&InputNode::FinalTransformUpdatedEvent>(*this);
	}
	void InputNode::SetSelectableFlag(bool selectable)
	{
		this->selectable = selectable;

		if (selectable == false)
			Unselect();
	}
	bool InputNode::IsSelected() const
	{
		return inputManager != nullptr && inputManager->GetSelectedNode() == this;
	}
	void InputNode::SetMouseMotionReportPolicy(MouseMotionReportPolicy policy)
	{
		this->mouseMotionReportPolicy = policy;
	}
	void InputNode::Unselect()
	{
		if (inputManager == nullptr)
			return;

		if (inputManager->GetSelectedNode() == this)
			inputManager->SelectNode(nullptr);
	}
	void InputNode::Select()
	{
		if (inputManager == nullptr)
			return;

		inputManager->SelectNode(this);
	}
	void InputNode::FinalTransformUpdatedEvent(const Node::FinalTransformUpdatedEvent& event)
	{
		if (inputManager != nullptr && event.node.IsEnabled())
			inputManager->UpdateNodeHitStatus(*this);
	}
	void InputNode::EnabledStateChangedEvent(const Node::EnabledStateChangedEvent& event)
	{
		if (IsEnabled())
		{
			if (inputManager)
				inputManager->UpdateNodeHitStatus(*this);
		}
		else if (mouseHitStatus != 0)
		{
			SetMouseHitStatus(0);
			Unselect();
		}
	}
	void InputNode::SurroundingNodeTreeChangedEvent(const Node::SurroundingNodeTreeChangedEvent& event)
	{
		if (event.type == Node::SurroundingNodeTreeChangedEvent::Type::ScreenChanged)
		{
			Unselect();
			SetMouseHitStatus(0);

			if (GetScreen() == nullptr)
			{
				inputManager = nullptr;
			}
			else
			{
				auto& newScreen = *GetScreen();
				inputManager = &InputManager::CreateScreenInputManager(newScreen);
				inputManager->UpdateNodeHitStatus(*this);
			}
		}
	}
	void InputNode::SetMouseHitStatus(int newMouseHitStatus)
	{
		int oldMouseHitStatus = mouseHitStatus;

		if (oldMouseHitStatus == newMouseHitStatus)
			return;

		mouseHitStatus = newMouseHitStatus;

		mouseHitStatusChangedEventDispatcher.Call({
				.node = *this,
				.timeNS = 0,
				.oldHitStatus = oldMouseHitStatus,
				.newHitStatus = newMouseHitStatus
			});
	}
}
