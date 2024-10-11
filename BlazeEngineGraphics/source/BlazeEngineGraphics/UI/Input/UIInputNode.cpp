#include "pch.h"
#include "BlazeEngineGraphics/UI/Input/UIInputNode.h"

namespace Blaze::UI
{
	InputNode::InputNode()
		: Node(), keyboardHandler(nullptr), mouseHandler(nullptr), selectHandler(nullptr), inputManagerArrayIndex(0), blocksMouseEvents(false)
	{

	}
	InputNode::~InputNode()
	{
	}
	void InputNode::SetKeyboardEventHandler(UIKeyboardEventHandler* keyboardEventHandler)
	{
		keyboardHandler = keyboardEventHandler;
	}
	void InputNode::SetMouseEventHandler(UIMouseEventHandler* mouseEventHandler)
	{
		mouseHandler = mouseEventHandler;
	}
	void InputNode::SetSelectEventHandler(UISelectEventHandler* selectEventHandler)
	{
		selectHandler = selectEventHandler;
	}
	void InputNode::SetBlocksMouseEventsFlag(bool blocksMouseEvents)
	{
		this->blocksMouseEvents = blocksMouseEvents;
	}
	bool InputNode::HitTest(Vec2f screenPosition)
	{
		auto finalTransform = GetFinalTransform();
		Vec2f pos = finalTransform.position;
		Vec2f size = finalTransform.size;
		float rot = finalTransform.rotation;
		float cos = Math::Cos(rot);
		float sin = Math::Sin(rot);
		Vec2f right = Vec2f(cos, sin) * size.x;
		Vec2f up = Vec2f(-sin, cos) * size.y;

		if (size.x == 0 || size.y == 0)
			return false;

		return Math::Shapes::Quad2Df({ Vec2f(), up, right + up, right }).IsInside(screenPosition - pos);
	}
}
