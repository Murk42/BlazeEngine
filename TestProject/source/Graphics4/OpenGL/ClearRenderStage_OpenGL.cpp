#include "pch.h"
#include "ClearRenderStage_OpenGL.h"
#include "OpenGLWrapper/OpenGLContext.h"

namespace Graphics4::OpenGL
{
	ClearRenderStage_OpenGL::ClearRenderStage_OpenGL(Graphics3::OpenGL::RenderWindow_OpenGL& renderWindow)		
		: renderWindow(renderWindow)
	{
	}
	ClearRenderStage_OpenGL::ClearRenderStage_OpenGL(ClearRenderStage_OpenGL&& other) noexcept
		: ClearRenderStageBase(std::move(other)), renderWindow(other.renderWindow)
	{
	}
	Array<Graphics3::Dynamic::SemaphoreBase*> ClearRenderStage_OpenGL::PerformStage(ArrayView<Graphics3::Dynamic::SemaphoreBase*> waitSemaphores, Graphics3::Dynamic::FramebufferBase& renderTarget)
	{
		PerformStage({ }, (Graphics3::OpenGL::Framebuffer_OpenGL&)renderTarget);
		return Array<Graphics3::Dynamic::SemaphoreBase*>();
	}
	Array<Graphics3::OpenGL::Semaphore_OpenGL*> ClearRenderStage_OpenGL::PerformStage(ArrayView<Graphics3::OpenGL::Semaphore_OpenGL*> waitSemaphores, Graphics3::OpenGL::Framebuffer_OpenGL& renderTarget)
	{
		uint framebufferID = renderTarget.GetID();

		if (framebufferID == 0)
		{
			Graphics3::OpenGL::RenderWindowFramebuffer_OpenGL& renderWindowFramebuffer = (Graphics3::OpenGL::RenderWindowFramebuffer_OpenGL&)renderTarget;

			SDL_GL_MakeCurrent((SDL_Window*)renderWindowFramebuffer.GetOpenGLRenderWindow()->GetWindowSDL()->GetHandle(), renderWindow.GetGraphicsContext()->GetContext());
		}

		OpenGLWrapper::ClearTarget();

		return Array<Graphics3::OpenGL::Semaphore_OpenGL*>();
	}
	ClearRenderStage_OpenGL& ClearRenderStage_OpenGL::operator=(ClearRenderStage_OpenGL&& other) noexcept
	{
		ClearRenderStageBase::operator=(std::move(other));

		return *this;
	}
}
