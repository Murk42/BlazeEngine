#pragma once
#include "Graphics4/Dynamic/ClearRenderStage.h"
#include "Graphics3/OpenGL/API_OpenGL.h"

namespace Graphics4::OpenGL
{
	class ClearRenderStage_OpenGL : public Dynamic::ClearRenderStageBase
	{
		Graphics3::OpenGL::RenderWindow_OpenGL& renderWindow;
	public:
		ClearRenderStage_OpenGL(Graphics3::OpenGL::RenderWindow_OpenGL& renderWindow);
		ClearRenderStage_OpenGL(ClearRenderStage_OpenGL&& other) noexcept;

		Array<Graphics3::Dynamic::SemaphoreBase*> PerformStage(ArrayView<Graphics3::Dynamic::SemaphoreBase*> waitSemaphores, Graphics3::Dynamic::FramebufferBase& renderTarget) override;
		Array<Graphics3::OpenGL::Semaphore_OpenGL*> PerformStage(ArrayView<Graphics3::OpenGL::Semaphore_OpenGL*> waitSemaphores, Graphics3::OpenGL::Framebuffer_OpenGL& renderTarget);

		ClearRenderStage_OpenGL& operator=(ClearRenderStage_OpenGL&& other) noexcept;
	};

	using ClearRenderStage = ClearRenderStage_OpenGL;
}