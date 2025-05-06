#pragma once
#include "BlazeEngineGraphics/UI/Core/Node.h"

namespace Blaze::UI::Nodes
{
	class BLAZE_GRAPHICS_API EmptyNode : public Node
	{
	public:
		int HitTest(Vec2f pos) override;
	};
}