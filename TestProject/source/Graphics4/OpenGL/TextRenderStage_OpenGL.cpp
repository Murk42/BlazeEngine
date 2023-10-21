#include "pch.h"
#include "TextRenderStage_OpenGL.h"

namespace Graphics4::OpenGL
{
    TextRenderStage_OpenGL::TextRenderStage_OpenGL(Graphics3::OpenGL::GraphicsContext_OpenGL& graphicsContext)
		: graphicsContext(graphicsContext)
    {
		OpenGLWrapper::VertexShader vert { "assets/shaders/quad.vert" };
		OpenGLWrapper::FragmentShader frag { "assets/shaders/quad.frag" };
		
		program.LinkShaders({ &vert, &frag }); 
		
		program.SetUniform(0, Mat4f::OrthographicMatrix(-2, 2, -2, 2, -1, 1));
		
		Vec3f vertices[]{
			Vec3f(-1, -1, 0),
			Vec3f(1, -1, 0),
			Vec3f(-1, 1, 0),
			Vec3f(-1, 1, 0),
			Vec3f(1, -1, 0),
			Vec3f(1, 1, 0),
		};
		
		vb.Allocate(vertices, sizeof(vertices));
		
		va.EnableVertexAttribute(0);
		va.SetVertexAttributeFormat(0, OpenGLWrapper::VertexAttributeType::Float, 3, false, 0);
		va.SetVertexAttributeBuffer(0, &vb, sizeof(Vec3f), 0);
    }

	TextRenderStage_OpenGL::TextRenderStage_OpenGL(TextRenderStage_OpenGL&& other) noexcept
		: TextRenderStageBase(std::move(other)), graphicsContext(other.graphicsContext), 
		program(std::move(other.program)), vb(std::move(other.vb)), va(std::move(other.va))
	{
	}

    Array<Graphics3::Dynamic::SemaphoreBase*> TextRenderStage_OpenGL::PerformStage(ArrayView<Graphics3::Dynamic::SemaphoreBase*> waitSemaphores, Graphics3::Dynamic::FramebufferBase& renderTarget)
    {
        PerformStage({ }, (Graphics3::OpenGL::Framebuffer_OpenGL&)renderTarget);
        return { };        
    }
    Array<Graphics3::OpenGL::Semaphore_OpenGL*> TextRenderStage_OpenGL::PerformStage(ArrayView<Graphics3::OpenGL::Semaphore_OpenGL*> waitSemaphores, Graphics3::OpenGL::Framebuffer_OpenGL& renderTarget)
    {
		uint framebufferID = renderTarget.GetID();

		if (framebufferID == 0)
		{
			Graphics3::OpenGL::RenderWindowFramebuffer_OpenGL& renderWindowFramebuffer = (Graphics3::OpenGL::RenderWindowFramebuffer_OpenGL&)renderTarget;
		
			SDL_GL_MakeCurrent((SDL_Window*)renderWindowFramebuffer.GetOpenGLRenderWindow()->GetWindowSDL()->GetHandle(), graphicsContext.GetContext());
		}
		
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebufferID);
		glUseProgram(program.GetHandle());
		glBindVertexArray(va.GetHandle());
		
		Vec2i size = renderTarget.GetSize();
		glViewport(0, 0, size.x, size.y);			
		
		glDrawArrays(GL_TRIANGLES, 0, 6);		        
        return { };
    }
	TextRenderStage_OpenGL& TextRenderStage_OpenGL::operator=(TextRenderStage_OpenGL&& other) noexcept
	{
		TextRenderStageBase::operator=(std::move(other));

		return *this;		
	}
}
