#include "pch.h"
#include "BlazeEngineGraphics/UI/Elements/UIImage.h"

namespace Blaze::UI
{
	Image::Image(Node* parent, const NodeTransform& transform)
		: Node(parent, transform), renderStream(this)
	{
		Node::renderStream = &renderStream;
	}
}