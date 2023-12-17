#include "pch.h"
#include "Line2DRenderTask_OpenGL.h"

namespace Graphics4::OpenGL
{
	Line2DRenderTask_OpenGL::Line2DRenderTask_OpenGL()		
		: renderer(nullptr)
	{
	}
	Line2DRenderTask_OpenGL::Line2DRenderTask_OpenGL(Line2DRenderTask_OpenGL&& other) noexcept
		: renderCache(std::move(other.renderCache)), renderer(other.renderer)
	{
		other.renderer = nullptr;
	}
	Array<Graphics4::Dynamic::SemaphoreBase*> Line2DRenderTask_OpenGL::PerformStage(ArrayView<Graphics4::Dynamic::SemaphoreBase*> waitSemaphores, Graphics4::Dynamic::FramebufferBase& renderTarget)
	{
		PerformStage({ }, (Graphics4::OpenGL::Framebuffer_OpenGL&)renderTarget);
		return { };		
	}
	Array<Graphics4::OpenGL::Semaphore_OpenGL*> Line2DRenderTask_OpenGL::PerformStage(ArrayView<Graphics4::OpenGL::Semaphore_OpenGL*> waitSemaphores, Graphics4::OpenGL::Framebuffer_OpenGL& renderTarget)
	{
		if (renderer != nullptr)
			renderer->Render(renderCache, renderTarget);
		return { };
	}
	Line2DRenderTask_OpenGL& Line2DRenderTask_OpenGL::operator=(Line2DRenderTask_OpenGL&& other) noexcept
	{		
		renderCache = std::move(other.renderCache);
		renderer = other.renderer;

		other.renderer = nullptr;

		return *this;
	}
}