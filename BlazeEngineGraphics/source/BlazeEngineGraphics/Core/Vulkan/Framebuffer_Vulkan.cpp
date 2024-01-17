#include "pch.h"
#include "BlazeEngineGraphics\Vulkan/Core/Framebuffer_Vulkan.h"

namespace Blaze::Graphics::Vulkan
{	
	Framebuffer_Vulkan::Framebuffer_Vulkan(VkImage image, VkFramebuffer handle, Vec2u size, RenderWindowFramebuffer_Vulkan* renderWindowFramebuffer)
		: handle(handle), image(image), size(size), renderWindowFramebuffer(renderWindowFramebuffer)
	{
	}
	Framebuffer_Vulkan::~Framebuffer_Vulkan()
	{		
	}
}