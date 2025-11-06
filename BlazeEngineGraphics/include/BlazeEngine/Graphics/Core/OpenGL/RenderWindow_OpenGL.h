#pragma once
#include "BlazeEngine/Core/Event/EventHandler.h"
#include "BlazeEngine/Runtime/Window.h"
#include "RenderWindowFramebuffer_OpenGL.h"

namespace Blaze::Graphics::OpenGL
{
	class GraphicsContext_OpenGL;
	class Semaphore_OpenGL;

	struct RenderWindowOptions_OpenGL
	{
		bool changeViewportAsWindowResizes = true;
		/*
			If this option is set to true then the window will be automatically be made current when created.
			Generally the first created render window for the given graphics context is made current, except if
			the graphics context was created with useSeparateInitWindow equal to true
		*/
		bool makeCurrent = true;
	};

	class BLAZE_API RenderWindow_OpenGL : public Window
	{
	public:
		/*
			The render window should be made active on the graphicsContext before rendering.
		*/
		/*Parity*/RenderWindow_OpenGL(GraphicsContext_OpenGL& graphicsContext, WindowCreateOptions windowOptions, RenderWindowOptions_OpenGL options = { });
		/*Parity*/~RenderWindow_OpenGL();

		/*Parity*/RenderWindowFramebuffer_OpenGL& AcquireNextFramebuffer(Semaphore_OpenGL* semaphore);
		/*Parity*/void PresentFramebuffer(RenderWindowFramebuffer_OpenGL& framebuffer, ArrayView<Semaphore_OpenGL*> waitSemaphores);

		/*Parity*/inline GraphicsContext_OpenGL& GetGraphicsContext() { return graphicsContext; }

		void Present();

		void ClearRenderBuffers();
		void ClearRenderColorBuffer();
		void ClearRenderDepthBuffer();
		void ClearRenderStencilBuffer();
	private:
		RenderWindowOptions_OpenGL options;
		RenderWindowFramebuffer_OpenGL framebuffer;
		GraphicsContext_OpenGL& graphicsContext;

		void Resized(const Window::WindowResizedEvent& event);
	};
}