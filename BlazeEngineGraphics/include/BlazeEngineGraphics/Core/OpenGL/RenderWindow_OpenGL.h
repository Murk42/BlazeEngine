#pragma once
#include "BlazeEngineCore/BlazeEngineCore.h"
#include "BlazeEngineCore/Event/EventHandler.h"
#include "BlazeEngine/Window/Window.h"
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "RenderWindowFramebuffer_OpenGL.h"

namespace Blaze::Graphics::OpenGL
{
	class GraphicsContext_OpenGL;	
	class Semaphore_OpenGL;			
	
	class BLAZE_GRAPHICS_API RenderWindow_OpenGL : private EventHandler<Window::WindowResizedEvent>
	{
	public:
		/*
			The render window should be made active on the graphicsContext before rendering.
		*/
		/*Parity*/RenderWindow_OpenGL(GraphicsContext_OpenGL& graphicsContext, WindowCreateOptions createOptions);
		/*Parity*/~RenderWindow_OpenGL();		

		/*Parity*/RenderWindowFramebuffer_OpenGL& AcquireNextFramebuffer(Semaphore_OpenGL* semaphore);
		/*Parity*/void PresentFramebuffer(RenderWindowFramebuffer_OpenGL& framebuffer, ArrayView<Semaphore_OpenGL*> waitSemaphores);

		/*Parity*/inline GraphicsContext_OpenGL& GetGraphicsContext() { return graphicsContext; }
		/*Parity*/inline Window& GetWindow() { return window; }				
		/*Parity*/Vec2u GetSize() const;				
	private:
		std::atomic_flag acquirable;

		RenderWindowFramebuffer_OpenGL framebuffer;
		GraphicsContext_OpenGL& graphicsContext;
		Window window;		

		void OnEvent(const Window::WindowResizedEvent& event) override;
	};
}