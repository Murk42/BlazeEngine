#pragma once

#if defined (GRAPHICS_OPENGL)
#include "OpenGL/API_OpenGL.h"

namespace Graphics4
{
	using GraphicsContext = OpenGL::GraphicsContext_OpenGL;
	using Semaphore = OpenGL::Semaphore_OpenGL;
	using Framebuffer = OpenGL::Framebuffer_OpenGL;
	using RenderWindowFramebuffer = OpenGL::RenderWindowFramebuffer_OpenGL;
	using RenderWindow = OpenGL::RenderWindow_OpenGL;

	using RenderPipeline = OpenGL::RenderPipeline_OpenGL;
}
#else
#include "Dynamic/API_dynamic.h"

namespace Graphics4
{
	using GraphicsContext = Dynamic::GraphicsContextBase;
	using Semaphore = Dynamic::SemaphoreBase;
	using Framebuffer = Dynamic::FramebufferBase;
	using RenderWindowFramebuffer = Dynamic::RenderWindowFramebufferBase;
	using RenderWindow = Dynamic::RenderWindowBase;

	using RenderPipeline = Dynamic::RenderPipelineBase;
}
#endif