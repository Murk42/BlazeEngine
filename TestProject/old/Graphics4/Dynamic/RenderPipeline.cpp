#include "pch.h"
#include "RenderPipeline.h"

namespace Graphics4::Dynamic
{
	RenderTaskBase::RenderTaskBase(GraphicsContextBase& graphicsContext)
		: GraphicsDynamicImplementation(graphicsContext)
	{
	}
	void RenderTaskBase::Execute(RenderTaskContextBase& renderContext)
	{
		if (auto impl = GetImplementation())
			impl->Execute(renderContext);
	}
	RenderPipelineBase::RenderPipelineBase(Graphics4::Dynamic::GraphicsContextBase& graphicsContext)
		: GraphicsDynamicImplementation(graphicsContext)
	{
	}
	void RenderPipelineBase::PerformPipeline(RenderPipelineContextBase& renderContext)
	{
		if (auto impl = GetImplementation())
			impl->PerformPipeline(renderContext);
	}	
}