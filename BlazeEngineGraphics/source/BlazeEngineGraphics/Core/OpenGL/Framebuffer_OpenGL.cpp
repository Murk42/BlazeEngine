#include "pch.h"
#include "BlazeEngineGraphics/Core/OpenGL/Framebuffer_OpenGL.h"
#include "BlazeEngineGraphics/Core/OpenGL/RenderWindow_OpenGL.h"

namespace Blaze::Graphics::OpenGL
{	
	Framebuffer_OpenGL::Framebuffer_OpenGL(RenderWindowFramebuffer_OpenGL* renderWindowFramebuffer)
		: id(0), renderWindowFramebuffer(renderWindowFramebuffer)
	{
	}
	Framebuffer_OpenGL::~Framebuffer_OpenGL()
	{		
	}		
}