#pragma once
#include "BlazeEngineGraphics/RenderStructure/StreamRenderer.h"

namespace Blaze::Graphics
{
	class RenderGroupStream
	{
	public:	
		virtual void BeginStream() = 0;
		virtual StreamRenderer* AdvanceGroup() = 0;
		virtual RenderStream* AdvanceStream() = 0;
	};
}