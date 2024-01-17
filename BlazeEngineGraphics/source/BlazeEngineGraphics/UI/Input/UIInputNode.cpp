#include "pch.h"
#include "BlazeEngineGraphics/UI/Input/UIInputNode.h"

namespace Blaze::UI
{
	UIInputNode::UIInputNode(Node* parent, const NodeTransform& transform)
		: Node(parent, transform)
	{

	}
	UIInputNode::~UIInputNode()
	{
	}
	void UIInputNode::SetKeyboardEventHandler(UIKeyboardEventHandler* keyboardEventHandler)
	{
		keyboardHandler = keyboardEventHandler;
	}
	void UIInputNode::SetMouseEventHandler(UIMouseEventHandler* mouseEventHandler)
	{
		mouseHandler = mouseEventHandler;
	}
	void UIInputNode::SetSelectEventHandler(UISelectEventHandler* selectEventHandler)
	{
		selectHandler = selectEventHandler;
	}
	void UIInputNode::SetHitPropagation(bool hitPropagates)
	{
		this->hitPropagates = hitPropagates;
	}
	bool UIInputNode::HitTest(Vec2f screenPosition)
	{
		Vec2f pos = GetFinalPosition();
		Vec2f size = GetFinalSize();
		float rot = GetFinalRotation();
		float cos = Math::Cos(rot);
		float sin = Math::Sin(rot);
		Vec2f right = Vec2f(cos, sin) * size.x;
		Vec2f up = Vec2f(-sin, cos) * size.y;

		if (size.x == 0 || size.y == 0)
			return false;

		return Math::Shapes::Quad2Df({ Vec2f(), up, right + up, right }).IsInside(screenPosition - pos);
	}
}
