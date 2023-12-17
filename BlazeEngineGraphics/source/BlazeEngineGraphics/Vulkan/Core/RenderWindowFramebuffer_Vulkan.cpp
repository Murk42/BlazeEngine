#include "pch.h"
#include "BlazeEngineGraphics\Vulkan/Core/RenderWindowFramebuffer_Vulkan.h"
#include "BlazeEngineGraphics\Vulkan/Core/Swapchain_Vulkan.h"
#include "BlazeEngineGraphics\Vulkan/Core/RenderWindow_Vulkan.h"

namespace Blaze::Graphics::Vulkan
{
	RenderWindow_Vulkan& RenderWindowFramebuffer_Vulkan::GetRenderWindow()
	{
		return swapchain.GetRenderWindow();
	}
	RenderWindowFramebuffer_Vulkan::RenderWindowFramebuffer_Vulkan(Swapchain_Vulkan& swapchain, uint imageIndex)
		: swapchain(swapchain), imageIndex(imageIndex), 
		framebuffer(
			(swapchain.GetImageCount() != 0 ? swapchain.GetImage(imageIndex) : VK_NULL_HANDLE),
			(swapchain.GetImageCount() != 0 ? swapchain.GetFramebuffer(imageIndex) : VK_NULL_HANDLE), swapchain.GetExtent(), this)
	{
	}
	RenderWindowFramebuffer_Vulkan::~RenderWindowFramebuffer_Vulkan()
	{						
	}
	void RenderWindowFramebuffer_Vulkan::SetImageIndex(uint newImageIndex)
	{
		imageIndex = newImageIndex;
		framebuffer.handle = swapchain.GetFramebuffer(imageIndex);
		framebuffer.image = swapchain.GetImage(imageIndex);
		framebuffer.size = swapchain.GetExtent();
	}	
}