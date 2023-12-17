#pragma once
#include "Graphics4/OpenGL/API_OpenGL.h"
#include "Graphics4/Dynamic/Line2DRenderTask.h"
#include "Graphics4/OpenGL/Line2DRenderer_OpenGL.h"

namespace Graphics4::OpenGL
{
	class Line2DRenderer_OpenGL;

	class Line2DRenderTask_OpenGL : public Dynamic::Line2DRenderTaskBase
	{
	public:
		Line2DRenderTask_OpenGL();
		Line2DRenderTask_OpenGL(const Line2DRenderTask_OpenGL&) = delete;
		Line2DRenderTask_OpenGL(Line2DRenderTask_OpenGL&& other) noexcept;

		Array<Graphics4::Dynamic::SemaphoreBase*> PerformStage(ArrayView<Graphics4::Dynamic::SemaphoreBase*> waitSemaphores, Graphics4::Dynamic::FramebufferBase& renderTarget) override;
		Array<Graphics4::OpenGL::Semaphore_OpenGL*> PerformStage(ArrayView<Graphics4::OpenGL::Semaphore_OpenGL*> waitSemaphores, Graphics4::OpenGL::Framebuffer_OpenGL& renderTarget);

		Line2DRenderTask_OpenGL& operator=(const Line2DRenderTask_OpenGL&) = delete;
		Line2DRenderTask_OpenGL& operator=(Line2DRenderTask_OpenGL&& other) noexcept;

		friend class Line2DRenderer_OpenGL;
	private:
		Line2DRenderer_OpenGL* renderer;
		Line2DRenderer_OpenGL::RenderCache renderCache;
	};
}