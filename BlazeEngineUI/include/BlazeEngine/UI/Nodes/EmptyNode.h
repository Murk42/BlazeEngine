#pragma once
#include "BlazeEngine/UI/Core/Node.h"

namespace Blaze::UI::Nodes
{
	class BLAZE_API EmptyNode : public Node
	{
	public:
		EmptyNode();
		EmptyNode(Node& node, const NodeTransform& transform);

		int HitTest(Vec2f pos) override;
	};
}