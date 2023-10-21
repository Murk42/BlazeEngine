#pragma once
#if defined(GRAPHICS_OPENGL)
#include "../OpenGL/API_OpenGL.h"
#elif defined(GRAPHICS_VULKAN)
#include "../Vulkan/API_Vulkan.h"
#else
#include "API_dynamic.h"
#endif

namespace Graphics3
{		
#if defined (GRAPHICS_OPENGL)

	using GraphicsContext = GraphicsContext_OpenGL;
	using Semaphore = Semaphore_OpenGL;
	using RenderContext = RenderContext_OpenGL;
	using Framebuffer = Framebuffer_OpenGL;	
	using RenderWindowFramebuffer = RenderWindowFramebuffer_OpenGL;
	using RenderWindow = RenderWindow_OpenGL;
#elif defined (GRAPHICS_VULKAN)
	using GraphicsContext = Vulkan::GraphicsContext_Vulkan;
	using Semaphore = Vulkan::Semaphore_Vulkan;
	using RenderContext = Vulkan::RenderContext_Vulkan;
	using Framebuffer = Vulkan::Framebuffer_Vulkan;	
	using RenderWindowFramebuffer = Vulkan::RenderWindowFramebuffer_Vulkan;
	using RenderWindow = Vulkan::RenderWindow_Vulkan;
#else
	using GraphicsContext = Dynamic::GraphicsContextBase;
	using Semaphore = Dynamic::SemaphoreBase;
	using RenderContext = Dynamic::RenderContextBase;
	using Framebuffer = Dynamic::FramebufferBase;	
	using RenderWindowFramebuffer = Dynamic::RenderWindowFramebufferBase;
	using RenderWindow = Dynamic::RenderWindowBase;
#endif
}