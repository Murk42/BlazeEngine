#pragma once
#include "BlazeEngineGraphics/RenderStructure/RenderGroupStream.h"

namespace Blaze::Graphics
{
	class RenderGroupRenderer
	{
	public:
		virtual void Render(RenderGroupStream& stream, Vec2u targetSize);
	};
}