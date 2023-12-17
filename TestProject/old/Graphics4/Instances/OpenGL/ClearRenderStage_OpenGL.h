#pragma once
#include "Graphics4/Dynamic/ClearRenderStage.h"
#include "Graphics4/OpenGL/API_OpenGL.h"

namespace Graphics4::OpenGL
{
	class ClearRenderStage_OpenGL : public Dynamic::ClearRenderStageBase
	{
		Graphics4::OpenGL::RenderWindow_OpenGL& renderWindow;
	public:
		ClearRenderStage_OpenGL(Graphics4::OpenGL::RenderWindow_OpenGL& renderWindow);
		ClearRenderStage_OpenGL(ClearRenderStage_OpenGL&& other) noexcept;

		Array<Graphics4::Dynamic::SemaphoreBase*> PerformStage(ArrayView<Graphics4::Dynamic::SemaphoreBase*> waitSemaphores, Graphics4::Dynamic::FramebufferBase& renderTarget) override;
		Array<Graphics4::OpenGL::Semaphore_OpenGL*> PerformStage(ArrayView<Graphics4::OpenGL::Semaphore_OpenGL*> waitSemaphores, Graphics4::OpenGL::Framebuffer_OpenGL& renderTarget);

		ClearRenderStage_OpenGL& operator=(ClearRenderStage_OpenGL&& other) noexcept;
	};	
}