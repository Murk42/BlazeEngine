#include "pch.h"
#include "BlazeEngineGraphics/Core/OpenGL/RenderWindow_OpenGL.h"
#include "BlazeEngineGraphics/Core/OpenGL/GraphicsContext_OpenGL.h"
#include "BlazeEngine/Console/Console.h"

namespace Blaze::Graphics::OpenGL
{
	RenderWindow_OpenGL::RenderWindow_OpenGL(GraphicsContext_OpenGL& graphicsContext, WindowCreateOptions createOptions)
		: graphicsContext(graphicsContext), framebuffer(*this)
	{				
		createOptions.graphicsAPI = WindowGraphicsAPI::OpenGL;
		window = Window(createOptions);
		window.windowResizedEventDispatcher.AddHandler(*this);

		std::atomic_flag_test_and_set(&acquirable);		

		framebuffer.framebuffer.size = window.GetSize();
	}

	RenderWindow_OpenGL::~RenderWindow_OpenGL()
	{
		window.windowResizedEventDispatcher.RemoveHandler(*this);
	}	

	RenderWindowFramebuffer_OpenGL& RenderWindow_OpenGL::AcquireNextFramebuffer(Semaphore_OpenGL* semaphore)
	{		
		return this->framebuffer;
	}
	void RenderWindow_OpenGL::PresentFramebuffer(RenderWindowFramebuffer_OpenGL& framebuffer, ArrayView<Semaphore_OpenGL*> waitSemaphores)
	{
		window.SwapBuffers();		
	}

	Vec2u RenderWindow_OpenGL::GetSize() const
	{		
		return framebuffer.framebuffer.size;		
	}	


	void RenderWindow_OpenGL::OnEvent(const Window::WindowResizedEvent& event)
	{					
		framebuffer.framebuffer.size = event.size;
	}
}