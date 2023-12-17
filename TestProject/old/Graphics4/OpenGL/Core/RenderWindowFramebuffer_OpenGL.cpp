#include "pch.h"
#include "RenderWindowFramebuffer_OpenGL.h"
#include "RenderWindow_OpenGL.h"

namespace Graphics4::OpenGL
{
	RenderWindowFramebuffer_OpenGL::RenderWindowFramebuffer_OpenGL()
		: renderWindow(nullptr)
	{
		framebuffer.renderWindowFramebuffer = this;
	}
	RenderWindowFramebuffer_OpenGL::RenderWindowFramebuffer_OpenGL(RenderWindowFramebuffer_OpenGL&& other) noexcept
		: RenderWindowFramebufferBase(std::move(other)), framebuffer(std::move(other.framebuffer))
	{
		renderWindow = other.renderWindow;
		other.renderWindow = nullptr;

		framebuffer.renderWindowFramebuffer = this;
	}
	RenderWindowFramebuffer_OpenGL::~RenderWindowFramebuffer_OpenGL()
	{
		framebuffer.id = -1;
		framebuffer.size = Vec2u();
	}
	bool RenderWindowFramebuffer_OpenGL::GetFramebuffer(Dynamic::FramebufferBase*& framebuffer)
	{
		framebuffer = &this->framebuffer;
		return true;
	}
	bool RenderWindowFramebuffer_OpenGL::GetFramebuffer(Framebuffer_OpenGL*& framebuffer)
	{
		framebuffer = &this->framebuffer;
		return true;
	}
	bool RenderWindowFramebuffer_OpenGL::GetRenderWindow(Dynamic::RenderWindowBase*& renderWindow) const
	{
		renderWindow = this->renderWindow;
		return true;
	}
	bool RenderWindowFramebuffer_OpenGL::GetRenderWindow(RenderWindow_OpenGL*& renderWindow) const
	{
		renderWindow = this->renderWindow;
		return true;
	}
	RenderWindowFramebuffer_OpenGL& RenderWindowFramebuffer_OpenGL::operator=(RenderWindowFramebuffer_OpenGL&& other) noexcept
	{
		RenderWindowFramebufferBase::operator=(std::move(other));

		renderWindow = other.renderWindow;
		framebuffer = std::move(other.framebuffer);
		other.renderWindow = nullptr;

		return *this;
	}
}