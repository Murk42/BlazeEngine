#pragma once
#include "BlazeEngineGraphics/RenderStructure/RenderStream.h"

namespace Blaze::Graphics
{	
	class StreamRenderer
	{
	public:
		virtual void Render(RenderStream& renderStream, Vec2u targetSize) = 0;

		virtual StringView GetRendererName() const = 0;
	};
}