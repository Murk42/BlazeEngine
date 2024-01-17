#pragma once
#include "RenderWindowFramebuffer_OpenGL.h"

namespace Blaze::Graphics::OpenGL
{
	class GraphicsContext_OpenGL;	
	class Semaphore_OpenGL;	

	class BLAZE_GRAPHICS_API RenderWindow_OpenGL : private EventHandler<Input::Events::WindowResizedEvent>
	{
	public:
		/*Parity*/RenderWindow_OpenGL(GraphicsContext_OpenGL& graphicsContext, WindowSDLCreateOptions_OpenGL createOptions);
		/*Parity*/~RenderWindow_OpenGL();		

		/*Parity*/RenderWindowFramebuffer_OpenGL& AcquireNextFramebuffer(Semaphore_OpenGL& semaphore);
		/*Parity*/void PresentFramebuffer(RenderWindowFramebuffer_OpenGL& framebuffer, Array<Semaphore_OpenGL*> waitSemaphores);

		/*Parity*/inline GraphicsContext_OpenGL& GetGraphicsContext() { return graphicsContext; }
		/*Parity*/inline WindowSDL& GetWindow() { return window; }		
		/*Parity*/Vec2u GetSize() const;

		void MakeActive();
	private:
		std::atomic_flag acquirable;

		RenderWindowFramebuffer_OpenGL framebuffer;
		GraphicsContext_OpenGL& graphicsContext;
		WindowSDL window;		

		void OnEvent(Input::Events::WindowResizedEvent event) override;
	};
}