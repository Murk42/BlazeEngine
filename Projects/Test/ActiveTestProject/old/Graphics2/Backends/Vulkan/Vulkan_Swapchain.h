#pragma once
#include "vulkan/vulkan.h"
#include "SDL/SDL_vulkan.h"
#include "../../Swapchain.h"
#include "Vulkan_Semaphore.h"

namespace Graphics2
{	
	class Vulkan_RenderContext;
	class Vulkan_Framebuffer;
	class Vulkan_Semaphore;

	class Vulkan_Swapchain : public Swapchain
	{
	public:
		using ContextType = Vulkan_RenderContext;
		
		Vulkan_RenderContext* renderContext;
		
		VkSwapchainKHR swapChain;
		VkRenderPass renderPass;

		int width;
		int height;

		uint32_t format;

		uint imageCount;
		Array<VkFramebuffer> swapChainFramebuffers;
		Array<VkImage> swapChainImages;
		Array<VkImageView> swapChainImageViews;
		Array<VkFence> fences;
		Array<Vulkan_Semaphore > semaphores;

		Array<Vulkan_Framebuffer> framebuffers;
		uint currentIndex;

		Vulkan_Swapchain(Vulkan_RenderContext* renderContext);		
		~Vulkan_Swapchain() override;

		void Create(RenderWindow* window) override;
		void Destroy() override;
		
		Framebuffer& AdvanceFramebuffers(Semaphore* semaphore) override;
	};
}
