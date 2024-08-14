#include "pch.h"
#include "BlazeEngineGraphics/Core/OpenGL/RenderWindow_OpenGL.h"
#include "BlazeEngineGraphics/Core/OpenGL/GraphicsContext_OpenGL.h"
#include "BlazeEngine/Console/Console.h"

namespace Blaze::Graphics::OpenGL
{
	RenderWindow_OpenGL::RenderWindow_OpenGL(GraphicsContext_OpenGL& graphicsContext, WindowSDLCreateOptions_OpenGL createOptions)
		: graphicsContext(graphicsContext), framebuffer(*this)
	{		
		window = graphicsContext.CreateWindowSDL(createOptions);				
		window.resizedEventDispatcher.AddHandler(*this);

		std::atomic_flag_test_and_set(&acquirable);		

		framebuffer.framebuffer.size = window.GetSize();
	}

	RenderWindow_OpenGL::~RenderWindow_OpenGL()
	{
		graphicsContext.DestroyWindowSDL(window);		
	}	

	RenderWindowFramebuffer_OpenGL& RenderWindow_OpenGL::AcquireNextFramebuffer(Semaphore_OpenGL& semaphore)
	{		
		return this->framebuffer;
	}
	void RenderWindow_OpenGL::PresentFramebuffer(RenderWindowFramebuffer_OpenGL& framebuffer, Array<Semaphore_OpenGL*> waitSemaphores)
	{
		SDL_GL_SwapWindow((SDL_Window*)window.GetHandle());
	}

	Vec2u RenderWindow_OpenGL::GetSize() const
	{
		int w, h;
		SDL_GL_GetDrawableSize((SDL_Window*)window.GetHandle(), &w, &h);
		return Vec2u(w, h);
	}	

	void RenderWindow_OpenGL::OnEvent(Input::Events::WindowResizedEvent event)
	{					
		framebuffer.framebuffer.size = event.size;
	}
}