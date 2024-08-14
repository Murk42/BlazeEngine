#include "pch.h"
#include "BlazeEngineGraphics/Core/OpenGL/RenderWindowFramebuffer_OpenGL.h"
#include "BlazeEngineGraphics/Core/OpenGL/RenderWindow_OpenGL.h"

namespace Blaze::Graphics::OpenGL
{	
	RenderWindowFramebuffer_OpenGL::~RenderWindowFramebuffer_OpenGL()
	{
		framebuffer.id = 0;
		framebuffer.size = Vec2u();
	}
	RenderWindowFramebuffer_OpenGL::RenderWindowFramebuffer_OpenGL(RenderWindow_OpenGL& renderWindow)
		: renderWindow(renderWindow), framebuffer(this)
	{
	}	
}