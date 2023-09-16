#include "EmptyRenderer_OpenGL.h"

namespace Graphics3::OpenGL
{
	EmptyRenderer_OpenGL::EmptyRenderer_OpenGL(GraphicsContext& graphicsContext, RenderContext& transferRenderContext)
	{		
	}
	void EmptyRenderer_OpenGL::RenderEmpty(RenderContext& context, Framebuffer& target, Recti renderArea, std::initializer_list<Semaphore*> waitSemaphores, std::initializer_list<Semaphore*> signalSemaphores)
	{				
	}
}