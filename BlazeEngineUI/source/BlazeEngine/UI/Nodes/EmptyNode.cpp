#include "pch.h"
#include "BlazeEngine/UI/Nodes/EmptyNode.h"

namespace Blaze::UI::Nodes
{
	EmptyNode::EmptyNode()
	{
		dataMap.SetTypeName("EmptyNode");
	}
	EmptyNode::EmptyNode(Node& node, const NodeTransform& transform)
		: EmptyNode()
	{
		SetParent(&node);
		SetTransform(transform);
	}
	int EmptyNode::HitTest(Vec2f pos)
	{
		return 0;
	}
}