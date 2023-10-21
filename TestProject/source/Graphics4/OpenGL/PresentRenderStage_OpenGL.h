#pragma once
#include "Graphics4/Dynamic/PresentRenderStage.h"
#include "Graphics3/OpenGL/API_OpenGL.h"

namespace Graphics4::OpenGL
{
	class PresentRenderStage_OpenGL : public Dynamic::PresentRenderStageBase
	{
		Graphics3::OpenGL::RenderWindow_OpenGL* renderWindow;
	public:
		PresentRenderStage_OpenGL(Graphics3::OpenGL::RenderWindow_OpenGL& renderWindow);

		Array<Graphics3::Dynamic::SemaphoreBase*> PerformStage(ArrayView<Graphics3::Dynamic::SemaphoreBase*> waitSemaphores, Graphics3::Dynamic::FramebufferBase& renderTarget) override;
		Array<Graphics3::OpenGL::Semaphore_OpenGL*> PerformStage(ArrayView<Graphics3::OpenGL::Semaphore_OpenGL*> waitSemaphores, Graphics3::OpenGL::Framebuffer_OpenGL& renderTarget);
	};

	using PresentRenderStage = PresentRenderStage_OpenGL;
}