#pragma once
#include "vulkan/vulkan.h"
#include "Vulkan_RenderContext.h"
#include "Vulkan_Framebuffer.h"
#include "../../Framebuffer.h"

namespace Experimental::Graphics::Vulkan
{	
	struct FramebufferImpl;
	struct SemaphoreImpl;	

	struct SwapchainImpl
	{
		SDL_Window* window;
		RenderContextImpl* renderContext;

		VkSurfaceKHR surface;
		VkSwapchainKHR swapChain;
		VkRenderPass renderPass;

		int width;
		int height;

		uint32_t format;

		Array<VkFramebuffer> swapChainFramebuffers;
		Array<VkImage> swapChainImages;
		Array<VkImageView> swapChainImageViews;

		Array<Framebuffer> framebuffers;

		SwapchainImpl(RenderContextImpl* renderContext, SDL_Window* window);
		~SwapchainImpl();

		Framebuffer& GetNextFramebuffer(SemaphoreImpl* signalSemaphore);
	};
}
