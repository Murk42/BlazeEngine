#include "pch.h"
#include "RenderPipeline_OpenGL.h"

namespace Graphics4::OpenGL
{
	RenderPipeline_OpenGL::RenderPipeline_OpenGL(Graphics4::OpenGL::GraphicsContext_OpenGL& graphicsContext)
	{
	}
	Array<Dynamic::SemaphoreBase*> RenderPipeline_OpenGL::PerformPipeline(Dynamic::FramebufferBase& renderTarget, ArrayView<Dynamic::SemaphoreBase*> waitSemaphores)
	{
		//Array<Graphics4::Dynamic::SemaphoreBase*> _waitSemaphores{ [&](Graphics4::Dynamic::SemaphoreBase** it, uint index) { *it = waitSemaphores[index]; }, (uint)waitSemaphores.Count() };		
		//Array<Graphics4::Dynamic::SemaphoreBase*> _signalSemaphores = RenderPipelineBase::PerformPipeline(renderTarget, _waitSemaphores);
		//return { [&](Dynamic::SemaphoreBase** it, uint index) { *it = (Dynamic::SemaphoreBase*)_waitSemaphores[index]; }, (uint)_waitSemaphores.Count() };
		return { };
	}	
}