#include "pch.h"
#include "BlazeEngineGraphics\OpenGL/Core/RenderWindowFramebuffer_OpenGL.h"
#include "BlazeEngineGraphics\OpenGL/Core/RenderWindow_OpenGL.h"

namespace Blaze::Graphics::OpenGL
{	
	RenderWindowFramebuffer_OpenGL::~RenderWindowFramebuffer_OpenGL()
	{
		framebuffer.id = -1;
		framebuffer.size = Vec2u();
	}
	RenderWindowFramebuffer_OpenGL::RenderWindowFramebuffer_OpenGL(RenderWindow_OpenGL& renderWindow)
		: renderWindow(renderWindow), framebuffer(0, renderWindow.GetSize(), this)
	{
	}
	void RenderWindowFramebuffer_OpenGL::ChangeSize(Vec2u size)
	{
		framebuffer.size = size;
	}
}