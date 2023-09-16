#include "Vulkan_Swapchain.h"
#include "Vulkan_Instance.h"
#include "Vulkan_Framebuffer.h"
#include "Vulkan_Semaphore.h"
#include <iostream>
#include <algorithm>

namespace Experimental::Graphics::Vulkan
{
    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        Array<VkSurfaceFormatKHR> formats;
        Array<VkPresentModeKHR> presentModes;
    };

    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, void* surface)
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

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const Array<VkSurfaceFormatKHR>& availableFormats)
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

    VkPresentModeKHR ChooseSwapPresentMode(const Array<VkPresentModeKHR>& availablePresentModes)
    {
        for (const auto& availablePresentMode : availablePresentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
        //return VK_PRESENT_MODE_IMMEDIATE_KHR;
    }

    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, int width, int height)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        }
        else {
            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

	SwapchainImpl::SwapchainImpl(RenderContextImpl* renderContext, SDL_Window* window)
        : renderContext(renderContext)
	{
        SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(renderContext->physicalDevice, surface);

        if (swapChainSupport.formats.Empty() || swapChainSupport.presentModes.Empty())
        {
            std::cout << "No supported swap chain!\n";
            std::exit(1);
        }

        int width, height;
        SDL_Vulkan_GetDrawableSize(window, &width, &height);

        VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities, width, height);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = (VkSurfaceKHR)surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        //QueueFamilyIndices indices = FindQueueFamilies((VkPhysicalDevice)renderer.physicalDevice);

        if (renderContext->presentQueueFamily != renderContext->graphicsQueueFamily) {
            uint32_t queueFamilyIndices[] = { renderContext->presentQueueFamily, renderContext->graphicsQueueFamily };

            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(renderContext->device, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
        {
            std::cout << "Failed to create swap chain!\n";
            std::exit(1);
        }

        this->format = surfaceFormat.format;

        this->width = extent.width;
        this->height = extent.height;

        vkGetSwapchainImagesKHR(renderContext->device, swapChain, &imageCount, nullptr);
        swapChainImages.Resize(imageCount);
        vkGetSwapchainImagesKHR(renderContext->device, swapChain, &imageCount, (VkImage*)swapChainImages.Ptr());

        swapChainImageViews.Resize(swapChainImages.Count());

        for (uint i = 0; i < swapChainImages.Count(); i++)
        {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = (VkImage)swapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = (VkFormat)format;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(renderContext->device, &createInfo, nullptr, (VkImageView*)&swapChainImageViews[i]) != VK_SUCCESS) {
                std::cout << "failed to create image views!\n";
                std::exit(1);
            }
        }

        //Creating render pass
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = (VkFormat)format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(renderContext->device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
            std::cout << "failed to create render pass!";
            std::exit(1);
        }

        //Creating framebuffers
        swapChainFramebuffers.Resize(swapChainImageViews.Count());        

        for (uint i = 0; i < swapChainImageViews.Count(); i++) {
            VkImageView attachments[] = {
                (VkImageView)swapChainImageViews[i]
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = (VkRenderPass)renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = width;
            framebufferInfo.height = height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(renderContext->device, &framebufferInfo, nullptr, (VkFramebuffer*)&swapChainFramebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }            
        }

        framebuffers.ResizeWithFunction(swapChainImageViews.Count(),
            [&](Framebuffer* framebuffer, uint index) {
                std::construct_at(framebuffer, new FramebufferImpl(swapChainFramebuffers[index]));
            });
	}

	SwapchainImpl::~SwapchainImpl()
	{
		if (vulkanInstance.surface != surface)
			vkDestroySurfaceKHR(vulkanInstance.instance, vulkanInstance.surface, nullptr);

		if (window == vulkanInstance.initRenderWindow)
		{
			window = nullptr;
			vulkanInstance.surfaceTaken = false;
		}

		for (uint i = 0; i < swapChainFramebuffers.Count(); i++) {
			vkDestroyFramebuffer(renderContext->device, swapChainFramebuffers[i], nullptr);
		}

		for (uint i = 0; i < swapChainImageViews.Count(); i++) {
			vkDestroyImageView(renderContext->device, swapChainImageViews[i], nullptr);
		}

		vkDestroySwapchainKHR(renderContext->device, swapChain, nullptr);

		vkDestroyRenderPass(renderContext->device, renderPass, nullptr);
	}

    Framebuffer& SwapchainImpl::GetNextFramebuffer(SemaphoreImpl* signalSemaphore)
    {
        uint32 imageIndex;
        VkResult result = vkAcquireNextImageKHR(renderContext->device, swapChain, UINT64_MAX, signalSemaphore->semaphore, VK_NULL_HANDLE, &imageIndex);

        /*if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            framebufferResized = false;
            RecreateSwapChain();
        }
        else */
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            std::cout << "failed to acquire swap chain image!\n";
            std::exit(1);
        }
        
        return framebuffers[imageIndex];
    }
}
