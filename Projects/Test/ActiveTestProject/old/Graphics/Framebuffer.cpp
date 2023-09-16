#include "Framebuffer.h"
#include "API Specific/Vulkan/Vulkan_Framebuffer.h"
#include "RenderContext.h"

namespace Experimental::Graphics
{
	using namespace Vulkan;

	Framebuffer::Framebuffer(void* impl)
		: impl(impl)
	{
	}

	Framebuffer::Framebuffer(RenderContext& renderContex)
	{
		//TODO implement creation
	}
	Framebuffer::~Framebuffer()
	{
		delete (FramebufferImpl*)impl;
	}
}