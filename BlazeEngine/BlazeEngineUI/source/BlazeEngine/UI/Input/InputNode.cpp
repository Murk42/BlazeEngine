#include "pch.h"
#include "BlazeEngine/UI/Input/InputSubSystem.h"
#include "BlazeEngine/UI/Input/InputNode.h"

namespace Blaze::UI
{
	InputNode::InputNode()
		: Node(), inputSubSystem(nullptr), selectable(true)
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
