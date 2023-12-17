#include "pch.h"
#include "ClearRenderStage_OpenGL.h"
#include "OpenGLWrapper/OpenGLContext.h"

namespace Graphics4::OpenGL
{
	ClearRenderStage_OpenGL::ClearRenderStage_OpenGL(Graphics4::OpenGL::RenderWindow_OpenGL& renderWindow)		
		: renderWindow(renderWindow)
	{
	}
	ClearRenderStage_OpenGL::ClearRenderStage_OpenGL(ClearRenderStage_OpenGL&& other) noexcept
		: ClearRenderStageBase(std::move(other)), renderWindow(other.renderWindow)
	{
	}
	Array<Graphics4::Dynamic::SemaphoreBase*> ClearRenderStage_OpenGL::PerformStage(ArrayView<Graphics4::Dynamic::SemaphoreBase*> waitSemaphores, Graphics4::Dynamic::FramebufferBase& renderTarget)
	{
		PerformStage({ }, (Graphics4::OpenGL::Framebuffer_OpenGL&)renderTarget);
		return Array<Graphics4::Dynamic::SemaphoreBase*>();
	}
	Array<Graphics4::OpenGL::Semaphore_OpenGL*> ClearRenderStage_OpenGL::PerformStage(ArrayView<Graphics4::OpenGL::Semaphore_OpenGL*> waitSemaphores, Graphics4::OpenGL::Framebuffer_OpenGL& renderTarget)
	{
		renderTarget.MakeActive();

		OpenGLWrapper::ClearTarget();

		return Array<Graphics4::OpenGL::Semaphore_OpenGL*>();
	}
	ClearRenderStage_OpenGL& ClearRenderStage_OpenGL::operator=(ClearRenderStage_OpenGL&& other) noexcept
	{
		ClearRenderStageBase::operator=(std::move(other));

		return *this;
	}
}
