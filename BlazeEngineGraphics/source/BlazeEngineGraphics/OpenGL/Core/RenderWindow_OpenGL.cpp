#include "pch.h"
#include "BlazeEngineGraphics\OpenGL/Core/RenderWindow_OpenGL.h"
#include "BlazeEngineGraphics\OpenGL/Core/GraphicsContext_OpenGL.h"
#include "BlazeEngine/Console/Console.h"

namespace Blaze::Graphics::OpenGL
{
	RenderWindow_OpenGL::RenderWindow_OpenGL(GraphicsContext_OpenGL& graphicsContext, StringView title, Vec2i pos, Vec2u size)
		: graphicsContext(graphicsContext), size(size), framebuffer(*this)
	{
		window = graphicsContext.CreateWindow(title, pos, size);
		window.resizedEventDispatcher.AddHandler(*this);

		std::atomic_flag_test_and_set(&acquirable);		
	}	

	RenderWindow_OpenGL::~RenderWindow_OpenGL()
	{
		graphicsContext.DestroyWindow(window);		
	}	

	RenderWindowFramebuffer_OpenGL& RenderWindow_OpenGL::AcquireNextFramebuffer(Semaphore_OpenGL& semaphore)
	{
		return this->framebuffer;
	}
	void RenderWindow_OpenGL::PresentFramebuffer(RenderWindowFramebuffer_OpenGL& framebuffer, Array<Semaphore_OpenGL*> waitSemaphores)
	{
		SDL_GL_SwapWindow((SDL_Window*)window.GetHandle());
	}

	void RenderWindow_OpenGL::MakeActive()
	{
		SDL_GL_MakeCurrent((SDL_Window*)window.GetHandle(), graphicsContext.GetContext());		
	}

	void RenderWindow_OpenGL::OnEvent(Input::Events::WindowResizedEvent event)
	{		
		size = event.size;
		framebuffer.ChangeSize(size);
	}
}