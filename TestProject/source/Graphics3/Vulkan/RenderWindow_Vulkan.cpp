#include "pch.h"
#include "API_Vulkan.h"

namespace Graphics3::Vulkan
{
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		Array<VkSurfaceFormatKHR> formats;
		Array<VkPresentModeKHR> presentModes;
	};

	static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, void* surface)
	{
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, (VkSurfaceKHR)surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, (VkSurfaceKHR)surface, &formatCount, nullptr);

		if (formatCount != 0) {
			details.formats.Resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, (VkSurfaceKHR)surface, &formatCount, details.formats.Ptr());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, (VkSurfaceKHR)surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.presentModes.Resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, (VkSurfaceKHR)surface, &presentModeCount, details.presentModes.Ptr());
		}

		return details;
	}

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
					return availablePresentMode;

			return VK_PRESENT_MODE_FIFO_KHR;
		}
		else
		{
			for (const auto& availablePresentMode : availablePresentModes)
				if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
					return availablePresentMode;

			Debug::Logger::LogWarning("Blaze Graphics API", "Requested non v-sync swapchain, but its not supported");

			return VK_PRESENT_MODE_FIFO_KHR;
		}
	}

	static VkExtent2D ChooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities, SDL_Window* window)
	{
		Vec2i requestedSize;
		SDL_Vulkan_GetDrawableSize(window, &requestedSize.x, &requestedSize.y);

		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}
		else {
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
		uint imageCount = capabilities.minImageCount + 1;

		if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
			imageCount = capabilities.maxImageCount;
		}

		return imageCount;
	}

	static VkRenderPass CreateCompatibleRenderPass(VkDevice device, VkFormat imageFormat)
	{
		VkRenderPass renderPass;

		VkAttachmentDescription attachmentDescriptions[1]{ };
		attachmentDescriptions[0].format = imageFormat;
		attachmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkSubpassDescription subpassDescriptions[1]{ };
		VkAttachmentReference subpassesAttachmentReferences[1][1]{ };

		subpassesAttachmentReferences[0][0].attachment = 0;
		subpassesAttachmentReferences[0][0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		subpassDescriptions[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescriptions[0].colorAttachmentCount = _countof(subpassesAttachmentReferences[0]);
		subpassDescriptions[0].pColorAttachments = subpassesAttachmentReferences[0];

		VkSubpassDependency subpassDependencies[1]{ };
		subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		subpassDependencies[0].srcAccessMask = 0;

		subpassDependencies[0].dstSubpass = 0;
		subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;;
		subpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = _countof(attachmentDescriptions);
		renderPassInfo.pAttachments = attachmentDescriptions;
		renderPassInfo.subpassCount = _countof(subpassDescriptions);
		renderPassInfo.pSubpasses = subpassDescriptions;
		renderPassInfo.dependencyCount = _countof(subpassDependencies);
		renderPassInfo.pDependencies = subpassDependencies;

		if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
			Debug::Logger::LogError("BlazeEngine", "Failed to create render pass!");

		return renderPass;
	}

	static uint CreateSwapchainResources(VkDevice device, VkSwapchainKHR swapchain, VkFormat format, Vec2i size, Array<Swapchain_Vulkan::ImageData>& imageData)
	{
		uint imageCount;

		Array<VkImage> images;
		vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
		images.Resize(imageCount);
		imageData.Resize(imageCount);
		vkGetSwapchainImagesKHR(device, swapchain, &imageCount, images.Ptr());		

		//Creating render pass		
		VkRenderPass renderPass = CreateCompatibleRenderPass(device, format);

		for (uint i = 0; i < imageCount; i++)
		{			
			imageData[i].image = images[i];

			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = imageData[i].image;
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = format;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(device, &createInfo, nullptr, &imageData[i].imageView) != VK_SUCCESS)
				Debug::Logger::LogFatal("Blaze Engine", "Failed to create image views!\n");

			VkImageView attachments[] = { imageData[i].imageView };

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = size.x;
			framebufferInfo.height = size.y;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &imageData[i].framebuffer) != VK_SUCCESS)
				Debug::Logger::LogFatal("Blaze Engine", "Failed to create framebuffer!\n");				
		}		

		vkDestroyRenderPass(device, renderPass, nullptr);

		return imageCount;
	}

	Swapchain_Vulkan::Swapchain_Vulkan()
		: graphicsContext(nullptr), renderWindow(nullptr), swapchain(VK_NULL_HANDLE), size(), format(), retired(false)
	{

	}
	Swapchain_Vulkan::Swapchain_Vulkan(RenderWindow_Vulkan& renderWindow, Swapchain_Vulkan* old)
		: graphicsContext(renderWindow.GetGraphicsContext()), renderWindow(&renderWindow), swapchain(VK_NULL_HANDLE), size(), format(), retired(false)
	{
		VkSharingMode sharingMode;

		Array<uint> queueFamilies;
		if (graphicsContext->GetGraphicsQueueFamily() == graphicsContext->GetPresentQueueFamily())
		{
			sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}
		else
		{
			sharingMode = VK_SHARING_MODE_CONCURRENT;
			queueFamilies = { graphicsContext->GetGraphicsQueueFamily(), graphicsContext->GetPresentQueueFamily() };
		}

		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(graphicsContext->GetPhysicalDevice(), renderWindow.GetSurface());

		if (swapChainSupport.formats.Empty() || swapChainSupport.presentModes.Empty())
			Debug::Logger::LogFatal("Blaze Engine", "No supported swap chain!\n");

		VkSurfaceFormatKHR surfaceFormat = ChooseSwapchainSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = ChooseSwapchainPresentMode(swapChainSupport.presentModes, false);
		VkExtent2D extent = ChooseSwapchainExtent(swapChainSupport.capabilities, (SDL_Window*)((WindowSDL*)renderWindow.GetWindow())->GetHandle());
		uint minImageCount = ChooseSwapchainMinImageCount(swapChainSupport.capabilities);

		if (extent.width == 0 || extent.height == 0)
			return;

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = renderWindow.GetSurface();
		createInfo.minImageCount = minImageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.imageSharingMode = sharingMode;
		createInfo.queueFamilyIndexCount = queueFamilies.Count();
		createInfo.pQueueFamilyIndices = queueFamilies.Ptr();
		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		if (old != nullptr)
		{
			createInfo.oldSwapchain = old->swapchain;
			old->retired = true;
		}
		else
			createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(graphicsContext->GetDevice(), &createInfo, nullptr, &swapchain) != VK_SUCCESS)
			Debug::Logger::LogFatal("Blaze Engine", "Failed to create swapchain!\n");

		swapchain = swapchain;
		format = surfaceFormat.format;
		size = { (int)extent.width, (int)extent.height };
		CreateSwapchainResources(graphicsContext->GetDevice(), swapchain, surfaceFormat.format, size, images);
	}
	Swapchain_Vulkan::~Swapchain_Vulkan()
	{
		Clear();
	}
	Swapchain_Vulkan::Swapchain_Vulkan(Swapchain_Vulkan&& other) noexcept
	{
		graphicsContext = other.graphicsContext;
		swapchain = other.swapchain;
		renderWindow = other.renderWindow;
		size = other.size;
		format = other.format;		
		images = std::move(other.images);		
		retired = other.retired;

		other.graphicsContext = nullptr;
		other.swapchain = VK_NULL_HANDLE;
		other.renderWindow = nullptr;
		other.size = Vec2i();
		other.format = { };		
		other.retired = false;
	}
	void Swapchain_Vulkan::Clear()
	{
		if (graphicsContext == VK_NULL_HANDLE)
			return;

		for (uint i = 0; i < images.Count(); i++)
		{
			vkDestroyFramebuffer(graphicsContext->GetDevice(), images[i].framebuffer, nullptr);
			vkDestroyImageView(graphicsContext->GetDevice(), images[i].imageView, nullptr);
		}		
		images.Clear();

		vkDestroySwapchainKHR(graphicsContext->GetDevice(), swapchain, nullptr);
		swapchain = NULL;

		graphicsContext = NULL;

		size = { };
		format = { };		
		retired = false;
	}
	Swapchain_Vulkan& Swapchain_Vulkan::operator=(Swapchain_Vulkan&& other) noexcept
	{
		Clear();

		graphicsContext = other.graphicsContext;
		swapchain = other.swapchain;
		renderWindow = other.renderWindow;
		size = other.size;
		format = other.format;
		images = std::move(other.images);
		retired = other.retired;

		other.graphicsContext = nullptr;
		other.swapchain = VK_NULL_HANDLE;
		other.renderWindow = nullptr;
		other.size = Vec2i();
		other.format = { };
		other.retired = false;

		return *this;
	}	

	RenderWindowFramebuffer_Vulkan::RenderWindowFramebuffer_Vulkan()
		: swapchain(nullptr), imageIndex(0)
	{		
	}

	RenderWindowFramebuffer_Vulkan::RenderWindowFramebuffer_Vulkan(RenderWindowFramebuffer_Vulkan&& other) noexcept
		: Framebuffer_Vulkan(std::move(other))
	{
		swapchain = other.swapchain;
		imageIndex = other.imageIndex;			

		other.swapchain = nullptr;
		other.imageIndex = 0;
	}

	RenderWindowFramebuffer_Vulkan::~RenderWindowFramebuffer_Vulkan()
	{
		Clear();
	}

	void RenderWindowFramebuffer_Vulkan::Clear()
	{
		handle = VK_NULL_HANDLE;
		size = Vec2i();
		swapchain = nullptr;
		imageIndex = 0;
		image = VK_NULL_HANDLE;
	}
	Vec2i RenderWindowFramebuffer_Vulkan::GetSize() const
	{
		return size;
	}
	RenderWindowFramebuffer_Vulkan::operator FramebufferBase&()
	{
		return (Framebuffer_Vulkan&)*this;
	}
	Dynamic::RenderWindowBase* RenderWindowFramebuffer_Vulkan::GetRenderWindow() const
	{
		return nullptr;
	}


	RenderWindowFramebuffer_Vulkan& RenderWindowFramebuffer_Vulkan::operator=(RenderWindowFramebuffer_Vulkan&& other) noexcept
	{
		Clear();
		Framebuffer_Vulkan::operator=(std::move(other));

		swapchain = other.swapchain;
		imageIndex = other.imageIndex;

		other.swapchain = nullptr;
		other.imageIndex = 0;

		return *this;
	}

	RenderWindow_Vulkan::RenderWindow_Vulkan(GraphicsContext_Vulkan* graphicsContext, StringView title, Vec2i pos, Vec2i size)
		: graphicsContext(graphicsContext), surface(NULL), framebufferIndex(0), newSwapchainRequired(false)
	{
		if (!graphicsContext)
			return;

		graphicsContext->CreateWindowAndSurface(window, surface, title, pos, size);
				
		window.resizedEventDispatcher.AddHandler(*this);
		window.minimizedEventDispatcher.AddHandler(*this);		

		CreateSwapchain();
	}

	RenderWindow_Vulkan::RenderWindow_Vulkan(RenderWindow_Vulkan&& other) noexcept : 
		EventHandler<Input::Events::WindowResizedEvent>(std::move(other)),
		EventHandler<Input::Events::WindowMinimizedEvent>(std::move(other))
	{
		graphicsContext = other.graphicsContext;
		surface = other.surface;
		window = std::move(other.window);		
		//TODO Check if moving of activeResources is okay
		retiredResources = std::move(other.retiredResources);
		activeResources = std::move(other.activeResources);
		framebufferIndex = other.framebufferIndex;
		newSwapchainRequired = other.newSwapchainRequired;
		size = other.size;		

		other.graphicsContext = NULL;
		other.surface = NULL;		
		other.framebufferIndex = 0;
		other.newSwapchainRequired = false;
		other.size = Vec2i();
	}

	RenderWindow_Vulkan::~RenderWindow_Vulkan()
	{
		Destroy();
	}

	void RenderWindow_Vulkan::Destroy()
	{
		if (!graphicsContext)
			return;		

		EventHandler<Input::Events::WindowResizedEvent>::UnsubscribeFromDispatcher();
		EventHandler<Input::Events::WindowMinimizedEvent>::UnsubscribeFromDispatcher();

		vkDeviceWaitIdle(graphicsContext->GetDevice());

		for (auto& resources : retiredResources)
		{
			for (auto& framebufferData : resources.framebuffersData)
			{				
				vkWaitForFences(graphicsContext->GetDevice(), 1, &framebufferData.fence, VK_TRUE, 0);
				vkDestroyFence(graphicsContext->GetDevice(), framebufferData.fence, nullptr);												
			}			
		}
		retiredResources.Clear(); 

		if (activeResources.swapchain.swapchain != VK_NULL_HANDLE)
		{
			for (auto& framebufferData : activeResources.framebuffersData)
			{
				vkWaitForFences(graphicsContext->GetDevice(), 1, &framebufferData.fence, VK_TRUE, 0);
				vkDestroyFence(graphicsContext->GetDevice(), framebufferData.fence, nullptr);
			}
			activeResources.retiredFrames = 0;
			activeResources.framebuffersData.Clear();
			activeResources.swapchain.Clear();
		}
		 				
		graphicsContext->DestroyWindowAndSurface(window, surface);

		newSwapchainRequired = false;
		size = { };
		framebufferIndex = 0;				
		surface = NULL;
		graphicsContext = NULL;		
	}

	Dynamic::RenderWindowFramebufferBase& RenderWindow_Vulkan::AcquireNextFramebuffer(Dynamic::SemaphoreBase& signalSemaphore)
	{		
		return AcquireNextFramebuffer((Semaphore_Vulkan&)signalSemaphore);
	}	
	RenderWindowFramebuffer_Vulkan& RenderWindow_Vulkan::AcquireNextFramebuffer(Semaphore_Vulkan& signalSemaphore)
	{								
		RenderWindowFramebuffer_Vulkan* currentFramebuffer = nullptr;
		VkFence* currentFence = nullptr;

		while (true)
		{			
			if (newSwapchainRequired)
				RecreateSwapchain();

			if (activeResources.swapchain.swapchain == VK_NULL_HANDLE)
				return activeResources.framebuffersData[0].framebuffer;

			currentFramebuffer = &activeResources.framebuffersData[framebufferIndex].framebuffer;
			currentFence = &activeResources.framebuffersData[framebufferIndex].fence;

			vkWaitForFences(graphicsContext->GetDevice(), 1, currentFence, VK_TRUE, UINT64_MAX);
			vkResetFences(graphicsContext->GetDevice(), 1, currentFence);

			VkResult result = vkAcquireNextImageKHR(graphicsContext->GetDevice(), activeResources.swapchain.swapchain, UINT64_MAX, signalSemaphore.GetHandle(), *currentFence, &currentFramebuffer->imageIndex);

			if (result == VK_ERROR_OUT_OF_DATE_KHR)
			{
				newSwapchainRequired = true;
				continue;
			}
			else if (result == VK_SUBOPTIMAL_KHR)
			{
				//newSwapChainRequired = true;
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

		currentFramebuffer->handle = activeResources.swapchain.images[currentFramebuffer->imageIndex].framebuffer;
		currentFramebuffer->size = activeResources.swapchain.size;
		currentFramebuffer->swapchain = &activeResources.swapchain;		
		currentFramebuffer->image = activeResources.swapchain.images[currentFramebuffer->imageIndex].image;

		framebufferIndex = (framebufferIndex + 1) % activeResources.framebuffersData.Count();

		return *currentFramebuffer;
	}

	//void RenderWindow_Vulkan::PresentRenderWindow(RenderContext_Vulkan& renderContext, RenderWindowFramebuffer_Vulkan& framebuffer, std::initializer_list<Semaphore_Vulkan*> waitSemaphores)
	//{
	//	if (framebuffer.swapchain == VK_NULL_HANDLE)
	//		return;
	//
	//	if (!framebuffer.swapchain->retired)
	//	{
	//		Array<VkSemaphore> _waitSemaphores { [&](VkSemaphore* it, uint index) {
	//			*it = waitSemaphores.begin()[index]->GetHandle();
	//		}, (uint)waitSemaphores.size()};
	//
	//		VkPresentInfoKHR presentInfo{ };
	//		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	//		presentInfo.pResults = nullptr;
	//		presentInfo.pSwapchains = &framebuffer.swapchain->swapchain;
	//		presentInfo.pImageIndices = &framebuffer.imageIndex;
	//		presentInfo.swapchainCount = 1;
	//		presentInfo.pWaitSemaphores = _waitSemaphores.Ptr();
	//		presentInfo.waitSemaphoreCount = _waitSemaphores.Count();
	//
	//		VkResult result = vkQueuePresentKHR(renderContext.GetQueue(), &presentInfo);
	//
	//		framebuffer.handle = VK_NULL_HANDLE;
	//		framebuffer.size = Vec2i();
	//		framebuffer.swapchain = nullptr;
	//		framebuffer.image = VK_NULL_HANDLE;
	//		framebuffer.imageIndex = 0;
	//
	//		if (result == VK_ERROR_OUT_OF_DATE_KHR)
	//			newSwapchainRequired = true;
	//		else if (result == VK_SUBOPTIMAL_KHR)
	//			newSwapchainRequired = true;
	//		else if (result != VK_SUCCESS)
	//			Debug::Logger::LogError("Blaze Engine", "Failed to present a swapchain!\n");
	//	}
	//
	//	CheckRetiredResources();
	//}

	RenderWindow_Vulkan& RenderWindow_Vulkan::operator=(RenderWindow_Vulkan&& other) noexcept
	{
		Destroy();		

		EventHandler<Input::Events::WindowResizedEvent>::operator=(std::move(other));
		EventHandler<Input::Events::WindowMinimizedEvent>::operator=(std::move(other));

		graphicsContext = other.graphicsContext;
		surface = other.surface;
		window = std::move(other.window);		
		//TODO Check if moving of activeResources is okay
		retiredResources = std::move(other.retiredResources);
		activeResources = std::move(other.activeResources);
		framebufferIndex = other.framebufferIndex;
		newSwapchainRequired = other.newSwapchainRequired;
		size = other.size;

		other.graphicsContext = NULL;
		other.surface = NULL;		
		other.framebufferIndex = 0;
		other.newSwapchainRequired = false;
		other.size = Vec2i();		

		return *this;
	}

	void RenderWindow_Vulkan::CreateSwapchain()
	{		
		activeResources.swapchain = Swapchain_Vulkan(*this, nullptr);
		CreateSwapchainResources(activeResources);		

		framebufferIndex = 0;
		newSwapchainRequired = false;
	}

	void RenderWindow_Vulkan::RecreateSwapchain()
	{		
		Swapchain_Vulkan* oldSwapchain;
		oldSwapchain = &activeResources.swapchain;

		if (oldSwapchain->swapchain != VK_NULL_HANDLE)
		{
			auto& oldResources = *retiredResources.AddBack(std::move(activeResources));

			for (auto& framebufferData : oldResources.framebuffersData)
				framebufferData.framebuffer.swapchain = &oldResources.swapchain;
		}
		
		activeResources.swapchain = Swapchain_Vulkan(*this, oldSwapchain);
		CreateSwapchainResources(activeResources);

		framebufferIndex = 0;
		newSwapchainRequired = false;
	}

	void RenderWindow_Vulkan::CreateSwapchainResources(SwapchainResources& resources)
	{
		if (resources.swapchain.size.x == 0 || resources.swapchain.size.y == 0)
		{
			resources.framebuffersData.Resize(1);
			resources.retiredFrames = 0;

			auto& framebufferData = resources.framebuffersData.First();

			framebufferData.framebuffer.swapchain = nullptr;
			framebufferData.framebuffer.imageIndex = 0;
			framebufferData.framebuffer.handle = VK_NULL_HANDLE;
			framebufferData.framebuffer.size = Vec2i();
			framebufferData.framebuffer.image = VK_NULL_HANDLE;

			framebufferData.fence = VK_NULL_HANDLE;
		}
		else
		{
			resources.framebuffersData.Resize(resources.swapchain.images.Count());
			resources.retiredFrames = 0;

			for (auto& framebufferData : resources.framebuffersData)
			{
				framebufferData.framebuffer.swapchain = nullptr;
				framebufferData.framebuffer.imageIndex = 0;
				framebufferData.framebuffer.handle = VK_NULL_HANDLE;
				framebufferData.framebuffer.size = Vec2i();
				framebufferData.framebuffer.image = VK_NULL_HANDLE;

				VkFenceCreateInfo fenceInfo{};
				fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
				fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

				if (vkCreateFence(graphicsContext->GetDevice(), &fenceInfo, nullptr, &framebufferData.fence) != VK_SUCCESS)
					Debug::Logger::LogFatal("Blaze Engine", "Failed to create fence\n");
			}
		}
	}

	void RenderWindow_Vulkan::CheckRetiredResources()
	{				
		retiredResources.EraseAll([&](SwapchainResources& resources)
			{				
				++resources.retiredFrames;
				if (resources.swapchain.images.Count() < resources.retiredFrames)
				{
					for (auto& data : resources.framebuffersData)
					{						
						//Unsignal the fence
						vkWaitForFences(graphicsContext->GetDevice(), 1, &data.fence, VK_TRUE, UINT64_MAX);

						vkDestroyFence(graphicsContext->GetDevice(), data.fence, nullptr);
						data.fence = VK_NULL_HANDLE;							
					}

					return true;
				}
				
				return false;
			});		
	}

	void RenderWindow_Vulkan::OnEvent(Input::Events::WindowResizedEvent event)
	{
		newSwapchainRequired = true;
		size = event.size;		
	}
	void RenderWindow_Vulkan::OnEvent(Input::Events::WindowMinimizedEvent event)
	{
		newSwapchainRequired = true;
		size = Vec2i();
	}
}