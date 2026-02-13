#include "pch.h"
#include "BlazeEngine/UI/Nodes/EmptyNode.h"

namespace Blaze::UI::Nodes
{
	EmptyNode::EmptyNode()
	{
		dataMap.SetTypeName("EmptyNode");
	}
	EmptyNode::EmptyNode(Node& node, const NodeTransform& transform)
		: Node(node, transform)
	{
	}
	Node::HitStatus EmptyNode::HitTest(Vec2f pos)
	{
		return HitStatus::NotHit;
	}
}