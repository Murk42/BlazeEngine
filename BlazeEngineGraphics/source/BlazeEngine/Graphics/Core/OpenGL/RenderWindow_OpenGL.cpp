#include "pch.h"
#include "BlazeEngine/Graphics/Core/OpenGL/RenderWindow_OpenGL.h"
#include "BlazeEngine/Graphics/Core/OpenGL/GraphicsContext_OpenGL.h"
#include "BlazeEngine/Core/Console/Console.h"
#include "BlazeEngine/External/SDL/SDL.h"

namespace Blaze::Graphics::OpenGL
{
	RenderWindow_OpenGL::RenderWindow_OpenGL(GraphicsContext_OpenGL& graphicsContext, WindowCreateOptions windowOptions, RenderWindowOptions_OpenGL options)
		: Window(graphicsContext.CreateWindow(windowOptions)), graphicsContext(&graphicsContext), framebuffer(*this), options(options)
	{
		resizedEventDispatcher.AddHandler<&RenderWindow_OpenGL::Resized>(*this);

		framebuffer.framebuffer.size = GetSize();

		if (options.makeCurrent)
			graphicsContext.SetActiveRenderWindow(*this);

		if (options.changeViewportAsWindowResizes && graphicsContext.GetActiveWindowHandle() == GetHandle())
			graphicsContext.SetRenderArea(Vec2i(), framebuffer.framebuffer.size);

	}
	RenderWindow_OpenGL::~RenderWindow_OpenGL()
	{
		Destroy();
	}
	void RenderWindow_OpenGL::Destroy()
	{
		resizedEventDispatcher.RemoveHandler<&RenderWindow_OpenGL::Resized>(*this);
		graphicsContext->RetrieveWindow(*this);
		this->Window::Destroy();
	}
	RenderWindowFramebuffer_OpenGL& RenderWindow_OpenGL::AcquireNextFramebuffer(Semaphore_OpenGL* semaphore)
	{
		return this->framebuffer;
	}
	void RenderWindow_OpenGL::PresentFramebuffer(RenderWindowFramebuffer_OpenGL& framebuffer, ArrayView<Semaphore_OpenGL*> waitSemaphores)
	{
		Present();
	}
	void RenderWindow_OpenGL::Present()
	{
		if (GetHandle()  == nullptr)
			BLAZE_LOG_FATAL("Window handle is nullptr");

		if (!SDL_GL_SwapWindow((SDL_Window*)GetHandle()))
			BLAZE_LOG_ERROR("SDL_GL_SwapWindow() failed. SDL_Error() returned: \"{}\"", SDL_GetError());
	}
	void RenderWindow_OpenGL::ClearRenderBuffers()
	{
		if (graphicsContext->GetActiveWindowHandle() != GetHandle())
			BLAZE_LOG_ERROR("Clearing render buffers of a RenderWindow_OpenGL that is not the active window in its GraphicsContext_OpenGL");
		else
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}
	void RenderWindow_OpenGL::ClearRenderColorBuffer()
	{
		if (graphicsContext->GetActiveWindowHandle() != GetHandle())
			BLAZE_LOG_ERROR("Clearing render buffers of a RenderWindow_OpenGL that is not the active window in its GraphicsContext_OpenGL");
		else
			glClear(GL_COLOR_BUFFER_BIT);
	}
	void RenderWindow_OpenGL::ClearRenderDepthBuffer()
	{
		if (graphicsContext->GetActiveWindowHandle() != GetHandle())
			BLAZE_LOG_ERROR("Clearing render buffers of a RenderWindow_OpenGL that is not the active window in its GraphicsContext_OpenGL");
		else
			glClear(GL_DEPTH_BUFFER_BIT);
	}
	void RenderWindow_OpenGL::ClearRenderStencilBuffer()
	{
		if (graphicsContext->GetActiveWindowHandle() != GetHandle())
			BLAZE_LOG_ERROR("Clearing render buffers of a RenderWindow_OpenGL that is not the active window in its GraphicsContext_OpenGL");
		else
			glClear(GL_STENCIL_BUFFER_BIT);
	}
	void RenderWindow_OpenGL::Resized(const Window::ResizedEvent& event)
	{
		graphicsContext->MakeContextActive();

		if (options.changeViewportAsWindowResizes && graphicsContext->GetActiveWindowHandle() == GetHandle())
			graphicsContext->SetRenderArea(Vec2i(), event.size);

		framebuffer.framebuffer.size = event.size;
	}
}