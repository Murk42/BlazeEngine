#include "pch.h"
#include "Framebuffer_OpenGL.h"
#include "GraphicsContext_OpenGL.h"
#include "RenderWindow_OpenGL.h"
#include "GL/glew.h"
#include "SDL2/SDL.h"

namespace Graphics4::OpenGL
{
	Framebuffer_OpenGL::Framebuffer_OpenGL()
		: id(-1), renderWindowFramebuffer(nullptr)
	{
	}
	Framebuffer_OpenGL::Framebuffer_OpenGL(Framebuffer_OpenGL&& other) noexcept
		: Dynamic::FramebufferBase(std::move(other)), id(other.id), size(other.size), renderWindowFramebuffer(other.renderWindowFramebuffer)
	{
		other.id = -1;
		other.size = Vec2u();
	}
	Framebuffer_OpenGL::~Framebuffer_OpenGL()
	{
		Destroy();
	}
	void Framebuffer_OpenGL::Destroy()
	{
		if (id != -1)
			glDeleteFramebuffers(1, &id);

		id = -1;
		size = Vec2u();
	}

	void Framebuffer_OpenGL::MakeActive() const
	{
		if (renderWindowFramebuffer != nullptr)
		{
			Graphics4::OpenGL::RenderWindow_OpenGL* renderWindow;
			renderWindowFramebuffer->GetRenderWindow(renderWindow);

			WindowSDL* window;
			renderWindow->GetWindow(window);

			SDL_GL_MakeCurrent((SDL_Window*)window->GetHandle(), renderWindow->GetGraphicsContext()->GetContext());

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		}
		else
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);
	}

	Framebuffer_OpenGL& Framebuffer_OpenGL::operator=(Framebuffer_OpenGL&& other) noexcept
	{
		Destroy();

		FramebufferBase::operator=(std::move(other));

		id = other.id;
		size = other.size;
		renderWindowFramebuffer = other.renderWindowFramebuffer;

		other.id = -1;
		other.size = Vec2u();
		other.renderWindowFramebuffer = nullptr;

		return *this;
	}
}