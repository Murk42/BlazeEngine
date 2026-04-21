#include "pch.h"
#include "BlazeEngine/UI/Input/InputSubSystem.h"
#include "BlazeEngine/UI/Input/InputNode.h"

namespace Blaze::UI
{
	InputNode::InputNode()
		: Node(), inputSubSystem(nullptr), selectable(true)
	{
	}
	InputNode::InputNode(Node& parent, const NodeTransform& transform)
		: Node(parent, transform), inputSubSystem(nullptr), selectable(true)
	{
	}
	InputNode::~InputNode()
	{
	}
	void InputNode::SetSelectableFlag(bool selectable)
	{
		this->selectable = selectable;

		if (selectable == false)
			Unselect();
	}
	bool InputNode::IsSelected() const
	{
		return inputSubSystem != nullptr && inputSubSystem->GetSelectedNode() == this;
	}
	bool InputNode::GetHitData(Input::MouseID mouseID, HitData& hitDataOut) const
	{
		if (inputSubSystem == nullptr)
			return false;

		const auto& pointerMap = inputSubSystem->GetPointerMap();

		auto it = pointerMap.Find(mouseID);
		if (it.IsNull())
			return false;

		hitDataOut = {
			InputNode::HitStatus::NotHit,
			it->value.GetPos()
		};

		auto hitData = it->value.GetHitData();

		for (auto& data : hitData)
			if (&data.node == this)
			{
				hitDataOut.status = data.hitStatus;
				break;
			}

		return true;
	}
	bool InputNode::CaptureMouse(Input::MouseID mouseID)
	{
		if (inputSubSystem != nullptr)
			return inputSubSystem->SetMouseCapturingNode(mouseID, this);
		else
			return false;
	}
	bool InputNode::ReleaseMouse(Input::MouseID mouseID)
	{
		if (inputSubSystem != nullptr && inputSubSystem->GetMouseCapturingNode(mouseID) == this)
			return inputSubSystem->SetMouseCapturingNode(mouseID, nullptr);
		else
			return false;
	}
	void InputNode::Unselect()
	{
		if (inputSubSystem == nullptr)
			return;

		if (inputSubSystem->GetSelectedNode() == this)
			inputSubSystem->SelectNode(nullptr);
	}
	void InputNode::Select()
	{
		if (inputSubSystem == nullptr)
			return;

		inputSubSystem->SelectNode(this); 
	}
}
