#pragma once
#include "Graphics3/OpenGL/API_OpenGL.h"
#include "Graphics4/Dynamic/TextRenderStage.h"
#include "OpenGLWrapper/OpenGLContext.h"

namespace Graphics4::OpenGL
{
	class TextRenderStage_OpenGL : public Dynamic::TextRenderStageBase
	{				
		Graphics3::OpenGL::GraphicsContext_OpenGL& graphicsContext;
		OpenGLWrapper::ShaderProgram program;
		OpenGLWrapper::ImmutableStaticGraphicsBuffer vb;
		OpenGLWrapper::VertexArray va;
	public:
		TextRenderStage_OpenGL(Graphics3::OpenGL::GraphicsContext_OpenGL& graphicsContext);
		TextRenderStage_OpenGL(TextRenderStage_OpenGL&& other) noexcept;

		Array<Graphics3::Dynamic::SemaphoreBase*> PerformStage(ArrayView<Graphics3::Dynamic::SemaphoreBase*> waitSemaphores, Graphics3::Dynamic::FramebufferBase& renderTarget) override;
		Array<Graphics3::OpenGL::Semaphore_OpenGL*> PerformStage(ArrayView<Graphics3::OpenGL::Semaphore_OpenGL*> waitSemaphores, Graphics3::OpenGL::Framebuffer_OpenGL& renderTarget);

		TextRenderStage_OpenGL& operator=(TextRenderStage_OpenGL&& other) noexcept;
	};
}