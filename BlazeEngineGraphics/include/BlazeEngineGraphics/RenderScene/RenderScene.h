#pragma once
#include "BlazeEngineGraphics/RenderScene/RenderObject.h"
#include "BlazeEngineGraphics/RenderStructure/RenderGroupRenderer.h"

namespace Blaze::Graphics
{
	class RenderScene
	{
	public:		
		virtual void Render(RenderGroupRenderer* renderGroupRenderer) = 0;
	};
}