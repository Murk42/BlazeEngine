#pragma once
#include "BlazeEngineGraphics/RenderScene/RenderUnit.h"

namespace Blaze::Graphics
{
	class RenderObject
	{
	public:
		virtual RenderUnit* GetRenderUnit(uint index) = 0;
	};
}