#include "pch.h"
#include "RenderWindow_OpenGL.h"
#include "GraphicsContext_OpenGL.h"

namespace Graphics4::OpenGL
{
	RenderWindow_OpenGL::RenderWindow_OpenGL(GraphicsContext_OpenGL& graphicsContext, StringView title, Vec2i pos, Vec2u size)
		: graphicsContext(&graphicsContext), size(size)
	{
		window = graphicsContext.CreateWindow(title, pos, size);
		window.resizedEventDispatcher.AddHandler(*this);

		std::atomic_flag_test_and_set(&acquirable);

		framebuffer.renderWindow = this;
		framebuffer.framebuffer.id = 0;
		framebuffer.framebuffer.size = size;
	}

	RenderWindow_OpenGL::RenderWindow_OpenGL(RenderWindow_OpenGL&& other) noexcept
		: EventHandler(std::move(other)), window(std::move(other.window)), framebuffer(std::move(other.framebuffer))
	{
		size = other.size;
		graphicsContext = other.graphicsContext;

		other.graphicsContext = nullptr;
		other.size = Vec2u();

		if (std::atomic_flag_test(&other.acquirable))
			std::atomic_flag_test_and_set(&acquirable);

		framebuffer.renderWindow = this;
		framebuffer.framebuffer.id = 0;
		framebuffer.framebuffer.size = size;
	}

	RenderWindow_OpenGL::~RenderWindow_OpenGL()
	{
		Destroy();
	}

	void RenderWindow_OpenGL::Destroy()
	{
		if (graphicsContext)
			graphicsContext->DestroyWindow(window);

		EventHandler::UnsubscribeFromDispatcher();

		graphicsContext = nullptr;
		size = Vec2u();
	}

	bool RenderWindow_OpenGL::AcquireNextFramebuffer(Dynamic::RenderWindowFramebufferBase*& framebuffer, Dynamic::SemaphoreBase& semaphore)
	{
		framebuffer = &this->framebuffer;
		return true;
	}

	bool RenderWindow_OpenGL::AcquireNextFramebuffer(RenderWindowFramebuffer_OpenGL*& framebuffer, Semaphore_OpenGL& semaphore)
	{
		framebuffer = &this->framebuffer;
		return true;
	}

	void RenderWindow_OpenGL::PresentFramebuffer(Dynamic::RenderWindowFramebufferBase& framebuffer, Array<Dynamic::SemaphoreBase*> waitSemaphores)
	{
		Array<Semaphore_OpenGL*> _waitSemaphores{ [&](auto it, auto index) { *it = (Semaphore_OpenGL*)waitSemaphores[index]; }, waitSemaphores.Count() };
		PresentFramebuffer((RenderWindowFramebuffer_OpenGL&)framebuffer, _waitSemaphores);
	}

	void RenderWindow_OpenGL::PresentFramebuffer(RenderWindowFramebuffer_OpenGL& framebuffer, Array<Semaphore_OpenGL*> waitSemaphores)
	{
		SDL_GL_SwapWindow((SDL_Window*)window.GetHandle());
	}

	RenderWindow_OpenGL& RenderWindow_OpenGL::operator=(RenderWindow_OpenGL&& other) noexcept
	{
		EventHandler::operator=(std::move(other));

		if (std::atomic_flag_test(&other.acquirable))
			std::atomic_flag_test_and_set(&acquirable);

		window = std::move(other.window);
		graphicsContext = other.graphicsContext;
		size = other.size;
		framebuffer = std::move(other.framebuffer);

		other.graphicsContext = nullptr;
		other.size = Vec2u();

		return *this;
	}

	void RenderWindow_OpenGL::OnEvent(Input::Events::WindowResizedEvent event)
	{
		size = event.size;
		framebuffer.framebuffer.size = size;
	}
}