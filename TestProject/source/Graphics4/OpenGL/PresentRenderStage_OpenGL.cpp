#include "pch.h"
#include "PresentRenderStage_OpenGL.h"
#include "GL/glew.h"

namespace Graphics4::OpenGL
{
	PresentRenderStage_OpenGL::PresentRenderStage_OpenGL(Graphics3::OpenGL::RenderWindow_OpenGL& renderWindow)
		: PresentRenderStageBase(), renderWindow(&renderWindow)
	{

	}
	Array<Graphics3::Dynamic::SemaphoreBase*> PresentRenderStage_OpenGL::PerformStage(ArrayView<Graphics3::Dynamic::SemaphoreBase*> waitSemaphores, Graphics3::Dynamic::FramebufferBase& renderTarget)
	{
		PerformStage({ }, (Graphics3::OpenGL::Framebuffer_OpenGL&)renderTarget);
		return { };
	}
	Array<Graphics3::OpenGL::Semaphore_OpenGL*> PresentRenderStage_OpenGL::PerformStage(ArrayView<Graphics3::OpenGL::Semaphore_OpenGL*> waitSemaphores, Graphics3::OpenGL::Framebuffer_OpenGL& renderTarget)
	{
		renderWindow->SwapWindowFramebuffers();
		return { };
	}
}