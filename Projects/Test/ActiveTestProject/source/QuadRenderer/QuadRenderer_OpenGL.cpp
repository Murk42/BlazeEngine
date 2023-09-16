#include "QuadRenderer_OpenGL.h"

namespace Graphics3::OpenGL
{
	QuadRenderer_OpenGL::QuadRenderer_OpenGL(GraphicsContext& graphicsContext, RenderContext& transferRenderContext)
	{
		//GraphicsLibrary::VertexShader vert { "assets/shaders/quad.vert" };
		//GraphicsLibrary::FragmentShader frag { "assets/shaders/quad.frag" };
		//
		//program.LinkShaders({ &vert, &frag }); 
		//
		//program.SetUniform(0, Mat4f::OrthographicMatrix(-2, 2, -2, 2, -1, 1));
		//
		//Vec3f vertices[]{
		//	Vec3f(-1, -1, 0),
		//	Vec3f(1, -1, 0),
		//	Vec3f(-1, 1, 0),
		//	Vec3f(-1, 1, 0),
		//	Vec3f(1, -1, 0),
		//	Vec3f(1, 1, 0),
		//};
		//
		//vb.Allocate(vertices, sizeof(vertices));
		//
		//va.EnableVertexAttribute(0);
		//va.SetVertexAttributeFormat(0, GraphicsLibrary::VertexAttributeType::Float, 3, false, 0);
		//va.SetVertexAttributeBuffer(0, &vb, sizeof(Vec3f), 0);
	}
	void QuadRenderer_OpenGL::RenderQuad(RenderContext& context, Framebuffer& target, std::initializer_list<Semaphore*> waitSemaphores, std::initializer_list<Semaphore*> signalSemaphores)
	{		
		//uint framebufferID = target.GetID();
		//if (framebufferID == 0)
		//{
		//	Graphics3::OpenGL::RenderWindowFramebuffer& renderWindowFramebuffer = (Graphics3::OpenGL::RenderWindowFramebuffer&)target;
		//
		//	SDL_GL_MakeCurrent((SDL_Window*)renderWindowFramebuffer.GetRenderWindow()->GetWindow()->GetHandle(), context.GetGraphicsContext()->GetContext());
		//}
		//
		//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebufferID);
		//glUseProgram(program.GetHandle());
		//glBindVertexArray(va.GetHandle());
		//
		//Vec2i size = target.GetSize();
		//glViewport(0, 0, size.x, size.y);
		//
		//Graphics::Renderer::ClearTarget();
		//
		//glDrawArrays(GL_TRIANGLES, 0, 6);		
	}
}