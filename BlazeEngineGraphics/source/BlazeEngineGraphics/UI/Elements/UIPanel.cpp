#include "pch.h"
#include "BlazeEngineGraphics/UI/Elements/UIPanel.h"

namespace Blaze::UI
{
	Panel::Panel(Node* parent, const NodeTransform& transform)
		: Node(parent, transform), renderStream(this)
	{
		Node::renderStream = &renderStream;
	}
}