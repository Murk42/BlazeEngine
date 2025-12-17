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
		SetTransform(transform);
		SetParent(&node);
	}
	Node::HitStatus EmptyNode::HitTest(Vec2f pos)
	{
		return HitStatus::NotHit;
	}
}