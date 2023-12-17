#pragma once
#include "Graphics4/OpenGL/API_OpenGL.h"
#include "Graphics4/Dynamic/TextRenderTask.h"
#include "Graphics4/OpenGL/TextRenderer_OpenGL.h"
#include "OpenGLWrapper/OpenGLContext.h"

namespace Graphics4::OpenGL
{

	class TextRenderTask_OpenGL : public Dynamic::TextRenderTaskBase
	{				
	public:
		TextRenderTask_OpenGL();
		TextRenderTask_OpenGL(const TextRenderTask_OpenGL&) = delete;
		TextRenderTask_OpenGL(TextRenderTask_OpenGL&& other) noexcept;

		Array<Graphics4::Dynamic::SemaphoreBase*> PerformStage(ArrayView<Graphics4::Dynamic::SemaphoreBase*> waitSemaphores, Graphics4::Dynamic::FramebufferBase& renderTarget) override;
		Array<Graphics4::OpenGL::Semaphore_OpenGL*> PerformStage(ArrayView<Graphics4::OpenGL::Semaphore_OpenGL*> waitSemaphores, Graphics4::OpenGL::Framebuffer_OpenGL& renderTarget);

		TextRenderTask_OpenGL& operator=(const TextRenderTask_OpenGL&) = delete;
		TextRenderTask_OpenGL& operator=(TextRenderTask_OpenGL&& other) noexcept;

		friend class TextRenderer_OpenGL;
	private:
		TextRenderer_OpenGL* renderer;
		TextRenderer_OpenGL::TextRenderCache renderCache;		
	};
}