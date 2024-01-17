#include "pch.h"
#include "BlazeEngineGraphics\Vulkan/Core/RenderWindow_Vulkan.h"
#include "BlazeEngineGraphics\Vulkan/Core/GraphicsContext_Vulkan.h"
#include "BlazeEngineGraphics\Vulkan/Core/Semaphore_Vulkan.h"
#include "SDL2/SDL_vulkan.h"
#include <algorithm>

namespace Blaze::Graphics::Vulkan
{	
	static VkSurfaceFormatKHR ChooseSwapchainSurfaceFormat(const Array<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}

		return availableFormats[0];
	}
	static VkPresentModeKHR ChooseSwapchainPresentMode(const Array<VkPresentModeKHR>& availablePresentModes, bool vSync)
	{
		if (vSync)
		{
			for (const auto& availablePresentMode : availablePresentModes)
				if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
					return VK_PRESENT_MODE_MAILBOX_KHR;

			return VK_PRESENT_MODE_FIFO_KHR;
		}
		else
		{
			for (const auto& availablePresentMode : availablePresentModes)
				if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
					return VK_PRESENT_MODE_IMMEDIATE_KHR;

			for (const auto& availablePresentMode : availablePresentModes)
				if (availablePresentMode == VK_PRESENT_MODE_FIFO_RELAXED_KHR)
					return VK_PRESENT_MODE_FIFO_RELAXED_KHR;

			Debug::Logger::LogWarning("Blaze Graphics API", "Requested non v-sync swapchain, but its not supported");

			return VK_PRESENT_MODE_FIFO_KHR;
		}
	}
	static VkExtent2D ChooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities, SDL_Window* window)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}
		else {
			Vec2i requestedSize;
			SDL_Vulkan_GetDrawableSize(window, &requestedSize.x, &requestedSize.y);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(requestedSize.x),
				static_cast<uint32_t>(requestedSize.y)
			};

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}
	static uint ChooseSwapchainMinImageCount(VkSurfaceCapabilitiesKHR capabilities)
	{
		uint imageCount = std::max(capabilities.minImageCount, 4Ui32);

		if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
			imageCount = capabilities.maxImageCount;
		}

		return imageCount;
	}

	RenderWindow_Vulkan::RenderWindow_Vulkan(GraphicsContext_Vulkan& graphicsContext, StringView title, Vec2i pos, Vec2u size)
		: graphicsContext(graphicsContext), surface(NULL), framebufferIndex(0), newSwapchainRequired(false), activeResources(nullptr)
	{		
		graphicsContext.CreateWindowAndSurface(window, surface, title, pos, size);

		this->size = window.GetSize();

		QuerrySwapchainSupportDetails();
		SetSwapchainsProperties();

		window.resizedEventDispatcher.AddHandler(*this);
		window.minimizedEventDispatcher.AddHandler(*this);

		CreateNewSwapchain();
	}	
	RenderWindow_Vulkan::~RenderWindow_Vulkan()
	{
		EventHandler<Input::Events::WindowResizedEvent>::UnsubscribeFromDispatcher();
		EventHandler<Input::Events::WindowMinimizedEvent>::UnsubscribeFromDispatcher();

		vkDeviceWaitIdle(graphicsContext.GetDevice());

		for (auto& resources : retiredResources)		
			delete resources;				

		if (activeResources)
			delete activeResources;

		graphicsContext.DestroyWindowAndSurface(window, surface);
	}		
	bool RenderWindow_Vulkan::AcquireNextFramebuffer(Semaphore_Vulkan& signalSemaphore, RenderWindowFramebuffer_Vulkan*& framebuffer)
	{
		RenderWindowFramebuffer_Vulkan* currentFramebuffer = nullptr;
		VkFence currentFence = VK_NULL_HANDLE;

		while (true)
		{
			if (newSwapchainRequired)			
				CreateNewSwapchain();		

			if (activeResources == nullptr)
				return false;

			auto& framebufferData = activeResources->GetFramebufferData(framebufferIndex);
			currentFramebuffer = &framebufferData.GetFramebuffer();
			currentFence = framebufferData.GetFence();

			vkWaitForFences(graphicsContext.GetDevice(), 1, &currentFence, VK_TRUE, UINT64_MAX);
			vkResetFences(graphicsContext.GetDevice(), 1, &currentFence);

			uint imageIndex;
			VkResult result = vkAcquireNextImageKHR(graphicsContext.GetDevice(), activeResources->GetSwapchain().GetHandle(), UINT64_MAX, signalSemaphore.GetHandle(), currentFence, &imageIndex);

			if (result == VK_ERROR_OUT_OF_DATE_KHR)
			{
				newSwapchainRequired = true;
				continue;
			}
			else if (result == VK_SUBOPTIMAL_KHR)
			{
				newSwapchainRequired = true;				
				//We exit the loop because the semaphore will be signaled
				break;
			}
			else if (result == VK_SUCCESS)
			{
				break;
			}
			else
			{
				Debug::Logger::LogFatal("Blaze Engine", "Failed to acquire swap chain image!\n");
				break;
			}
		}		

		framebufferIndex = (framebufferIndex + 1) % activeResources->GetSwapchain().GetImageCount();

		framebuffer = currentFramebuffer;

		return true;
	}	
	void RenderWindow_Vulkan::PresentFramebuffer(RenderWindowFramebuffer_Vulkan& framebuffer, Array<Semaphore_Vulkan*> waitSemaphores)
	{		
		VkQueue queue;
		vkGetDeviceQueue(graphicsContext.GetDevice(), graphicsContext.GetPresentQueueFamily(), 0, &queue);		

		if (!framebuffer.GetSwapchain().IsRetired())
		{
			Array<VkSemaphore> _waitSemaphores{ waitSemaphores.Count(), [&](auto it, auto index) {
				*it = waitSemaphores[index]->GetHandle();
			} };

			VkSwapchainKHR swapchain = framebuffer.GetSwapchain().GetHandle();
			uint imageIndex = framebuffer.GetImageIndex();

			VkPresentInfoKHR presentInfo{ };
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.pResults = nullptr;
			presentInfo.pSwapchains = &swapchain;
			presentInfo.pImageIndices = &imageIndex;
			presentInfo.swapchainCount = 1;
			presentInfo.pWaitSemaphores = _waitSemaphores.Ptr();
			presentInfo.waitSemaphoreCount = _waitSemaphores.Count();

			VkResult result = vkQueuePresentKHR(queue, &presentInfo);			

			if (result == VK_ERROR_OUT_OF_DATE_KHR)
				newSwapchainRequired = true;
			else if (result == VK_SUBOPTIMAL_KHR)
				newSwapchainRequired = true;
			else if (result != VK_SUCCESS)
				Debug::Logger::LogError("Blaze Engine", "Failed to present a swapchain!\n");
		}

		CheckRetiredResources();
	}	
	void RenderWindow_Vulkan::QuerrySwapchainSupportDetails()
	{
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(graphicsContext.GetPhysicalDevice(), surface, &swapchainSupportDetails.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(graphicsContext.GetPhysicalDevice(), surface, &formatCount, nullptr);

		if (formatCount != 0) {
			swapchainSupportDetails.formats.Resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(graphicsContext.GetPhysicalDevice(), surface, &formatCount, swapchainSupportDetails.formats.Ptr());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(graphicsContext.GetPhysicalDevice(), surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			swapchainSupportDetails.presentModes.Resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(graphicsContext.GetPhysicalDevice(), surface, &presentModeCount, swapchainSupportDetails.presentModes.Ptr());
		}

		if (swapchainSupportDetails.formats.Empty() || swapchainSupportDetails.presentModes.Empty())
			Debug::Logger::LogFatal("Blaze Engine", "No supported swap chain!\n");
	}
	void RenderWindow_Vulkan::SetSwapchainsProperties()
	{				
		if (graphicsContext.GetGraphicsQueueFamily() == graphicsContext.GetPresentQueueFamily())
		{
			swapchainsSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapchainsQueueFamilies = { };
		}
		else
		{
			swapchainsSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapchainsQueueFamilies = { graphicsContext.GetGraphicsQueueFamily(), graphicsContext.GetPresentQueueFamily() };
		}			

		surfaceFormat = ChooseSwapchainSurfaceFormat(swapchainSupportDetails.formats);
		presentMode = ChooseSwapchainPresentMode(swapchainSupportDetails.presentModes, false);
		imageCount = ChooseSwapchainMinImageCount(swapchainSupportDetails.capabilities);
		swapchainTransform = swapchainSupportDetails.capabilities.currentTransform;
		extent = ChooseSwapchainExtent(swapchainSupportDetails.capabilities, (SDL_Window*)window.GetHandle());		
	}	
	void RenderWindow_Vulkan::CreateNewSwapchain()
	{
		Swapchain_Vulkan* oldSwapchain = nullptr;

		if (activeResources != nullptr)
		{
			oldSwapchain = &activeResources->GetSwapchain();
			retiredResources.AddBack(activeResources);
		}
		
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(graphicsContext.GetPhysicalDevice(), surface, &swapchainSupportDetails.capabilities);

		imageCount = ChooseSwapchainMinImageCount(swapchainSupportDetails.capabilities);
		swapchainTransform = swapchainSupportDetails.capabilities.currentTransform;
		extent = ChooseSwapchainExtent(swapchainSupportDetails.capabilities, (SDL_Window*)window.GetHandle());

		if (extent.width == 0 || extent.height == 0)
			activeResources = nullptr;
		else
			activeResources = new SwapchainResources(*this, oldSwapchain);		

		framebufferIndex = 0;
		newSwapchainRequired = false;
	}	
	void RenderWindow_Vulkan::CheckRetiredResources()
	{		
		retiredResources.EraseAll([&](SwapchainResources*& resources)
			{
				bool toErase = resources->UpdateRetired();
				if (toErase)
					delete resources;
				return toErase;
			});
	}
	void RenderWindow_Vulkan::OnEvent(Input::Events::WindowResizedEvent event)
	{
		newSwapchainRequired = true;
		size = event.size;
		extent = ChooseSwapchainExtent(swapchainSupportDetails.capabilities, (SDL_Window*)window.GetHandle());
	}
	void RenderWindow_Vulkan::OnEvent(Input::Events::WindowMinimizedEvent event)
	{
		newSwapchainRequired = true;
		size = Vec2u();
		extent = { 0, 0 };
	}

	RenderWindow_Vulkan::FramebufferData::FramebufferData(Swapchain_Vulkan& swapchain, uint imageIndex)
		: framebuffer(swapchain, imageIndex)
	{
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		if (vkCreateFence(swapchain.GetGraphicsContext().GetDevice(), &fenceInfo, nullptr, &fence) != VK_SUCCESS)
			Debug::Logger::LogFatal("Blaze Engine", "Failed to create fence\n");
	}
	RenderWindow_Vulkan::FramebufferData::~FramebufferData()
	{
		vkWaitForFences(framebuffer.GetSwapchain().GetGraphicsContext().GetDevice(), 1, &fence, VK_TRUE, 0);
		vkDestroyFence(framebuffer.GetSwapchain().GetGraphicsContext().GetDevice(), fence, nullptr);
	}

	RenderWindow_Vulkan::SwapchainResources::SwapchainResources(RenderWindow_Vulkan& renderWindow, Swapchain_Vulkan* oldSwapchain)
		: swapchain(renderWindow, oldSwapchain)
	{		
		if (swapchain.GetExtent().x == 0 || swapchain.GetExtent().y == 0)
			framebuffersData.Resize(1, swapchain, 0);
		else		
			framebuffersData.ResizeWithFunction(swapchain.GetImageCount(), [&](FramebufferData* it, uintMem index) {
				new (it) FramebufferData(swapchain, index);
				});		
		retiredFrames = 0;			
	}
	RenderWindow_Vulkan::SwapchainResources::~SwapchainResources()
	{				
	}
	bool RenderWindow_Vulkan::SwapchainResources::UpdateRetired()
	{
		++retiredFrames;

		//wait n + 1 frames until destruction
		if (swapchain.GetImageCount() + 1 == retiredFrames)				
			return true;		

		return false;
	}
}