#pragma once
#include "BlazeEngineCore/DataStructures/StringView.h"
#include "BlazeEngineCore/Math/Vector.h"
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