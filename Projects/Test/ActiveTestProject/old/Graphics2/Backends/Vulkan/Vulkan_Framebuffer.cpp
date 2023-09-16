#include "Vulkan_Framebuffer.h"

namespace Graphics2
{	
	Vulkan_Framebuffer::Vulkan_Framebuffer(Vulkan_RenderContext* renderContext)
		: renderContext(renderContext)
	{
	}
	Vulkan_Framebuffer::Vulkan_Framebuffer(VkFramebuffer handle)
	{
	}
	Vulkan_Framebuffer::~Vulkan_Framebuffer()
	{
	}
}