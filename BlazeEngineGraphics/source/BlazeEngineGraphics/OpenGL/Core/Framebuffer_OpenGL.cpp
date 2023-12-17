#include "pch.h"
#include "BlazeEngineGraphics\OpenGL/Core/Framebuffer_OpenGL.h"
#include "BlazeEngineGraphics\OpenGL/Core/GraphicsContext_OpenGL.h"
#include "BlazeEngineGraphics\OpenGL/Core/RenderWindow_OpenGL.h"

namespace Blaze::Graphics::OpenGL
{	
	Framebuffer_OpenGL::Framebuffer_OpenGL(uint id, Vec2u size, RenderWindowFramebuffer_OpenGL* renderWindowFramebuffer)
		: id(id), size(size), renderWindowFramebuffer(renderWindowFramebuffer)
	{
	}
	Framebuffer_OpenGL::~Framebuffer_OpenGL()
	{		
	}		
}