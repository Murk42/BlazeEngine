#include "pch.h"
#include "RenderPipeline_OpenGL.h"

namespace Graphics4::OpenGL
{
	RenderPipeline_OpenGL::RenderPipeline_OpenGL(Graphics3::OpenGL::GraphicsContext_OpenGL& graphicsContext)
	{
	}
	Array<Graphics3::Semaphore*> RenderPipeline_OpenGL::PerformPipeline(Graphics3::Framebuffer& renderTarget, ArrayView<Graphics3::Semaphore*> waitSemaphores)
	{
		Array<Graphics3::Dynamic::SemaphoreBase*> _waitSemaphores{ [&](Graphics3::Dynamic::SemaphoreBase** it, uint index) { *it = waitSemaphores[index]; }, (uint)waitSemaphores.Count() };		
		Array<Graphics3::Dynamic::SemaphoreBase*> _signalSemaphores = RenderPipelineBase::PerformPipeline(renderTarget, _waitSemaphores);
		return { [&](Graphics3::Semaphore** it, uint index) { *it = (Graphics3::Semaphore*)_waitSemaphores[index]; }, (uint)_waitSemaphores.Count() };
	}
	void RenderPipeline_OpenGL::TransitionGraphicsResource(GraphicsResource* resource, GraphicsResourceLayout source, GraphicsResourceLayout destination)
	{
	}
}