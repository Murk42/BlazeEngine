#include "Vulkan_Framebuffer.h"

namespace Experimental::Graphics::Vulkan
{
	FramebufferImpl::FramebufferImpl()
		: renderContext(nullptr), framebuffer(nullptr)
	{
	}
	FramebufferImpl::FramebufferImpl(VkFramebuffer framebuffer)
		: renderContext(nullptr), framebuffer(framebuffer)
	{
	}
	FramebufferImpl::~FramebufferImpl()
	{
		if (renderContext != nullptr)

			;//TODO: Implement framebuffer deletion
	}
}