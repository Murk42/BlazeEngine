#pragma once
#include "BlazeEngineGraphics/Core/RenderStream.h"

namespace Blaze::Graphics
{	
	class StreamRenderer
	{
	public:
		virtual void Render(RenderStream& renderStream, Vec2u targetSize) = 0;
	};
}