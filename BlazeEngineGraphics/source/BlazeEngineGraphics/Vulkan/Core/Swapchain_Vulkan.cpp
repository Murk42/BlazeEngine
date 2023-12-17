#include "pch.h"
#include "BlazeEngineGraphics\Vulkan/Core/Swapchain_Vulkan.h"
#include "BlazeEngineGraphics\Vulkan/Core/GraphicsContext_Vulkan.h"
#include "BlazeEngineGraphics\Vulkan/Core/RenderWindow_Vulkan.h"
#include <algorithm>
#include "SDL2/SDL_vulkan.h"

namespace Blaze::Graphics::Vulkan
{		
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


	Swapchain_Vulkan::Swapchain_Vulkan(RenderWindow_Vulkan& renderWindow, Swapchain_Vulkan* old)
		: graphicsContext(renderWindow.GetGraphicsContext()), renderWindow(renderWindow), swapchain(VK_NULL_HANDLE), extent(), format(), retired(false)
	{				
		auto queueFamilies = renderWindow.GetSwapchainsQueueFamilies();
		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = renderWindow.GetSurface();
		createInfo.minImageCount = renderWindow.GetImageCount();
		createInfo.imageFormat = renderWindow.GetSurfaceFormat().format;
		createInfo.imageColorSpace = renderWindow.GetSurfaceFormat().colorSpace;
		createInfo.imageExtent = renderWindow.GetExtent();
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.imageSharingMode = renderWindow.GetSwapchainsSharingMode();
		createInfo.queueFamilyIndexCount = queueFamilies.Count();
		createInfo.pQueueFamilyIndices = queueFamilies.Ptr();
		createInfo.preTransform = renderWindow.GetSwapchainTransform();
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = renderWindow.GetPresentMode();
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = (old != nullptr ? old->swapchain : VK_NULL_HANDLE);

		if (old != nullptr)
			old->retired = true;

		if (vkCreateSwapchainKHR(graphicsContext.GetDevice(), &createInfo, nullptr, &swapchain) != VK_SUCCESS)
			Debug::Logger::LogFatal("Blaze Engine", "Failed to create swapchain!\n");

		format = renderWindow.GetSurfaceFormat().format;	
		extent = { renderWindow.GetExtent().width, renderWindow.GetExtent().height };

		CreateResources();		
	}
	Swapchain_Vulkan::~Swapchain_Vulkan()
	{			
		for (uint i = 0; i < images.Count(); i++)
		{
			vkDestroyFramebuffer(graphicsContext.GetDevice(), images[i].framebuffer, nullptr);
			vkDestroyImageView(graphicsContext.GetDevice(), images[i].imageView, nullptr);
		}

		images.Clear();
		
		vkDestroySwapchainKHR(graphicsContext.GetDevice(), swapchain, nullptr);		
	}
	void Swapchain_Vulkan::CreateResources()
	{
		uint imageCount;

		Array<VkImage> imageHandles;
		vkGetSwapchainImagesKHR(graphicsContext.GetDevice(), swapchain, &imageCount, nullptr);
		imageHandles.Resize(imageCount);

		images.Resize(imageCount);
		vkGetSwapchainImagesKHR(graphicsContext.GetDevice(), swapchain, &imageCount, imageHandles.Ptr());

		//Creating render pass		
		VkRenderPass renderPass = CreateCompatibleRenderPass(graphicsContext.GetDevice(), format);

		for (uint i = 0; i < imageCount; i++)
		{
			images[i].image = imageHandles[i];

			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = images[i].image;
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

			if (vkCreateImageView(graphicsContext.GetDevice(), &createInfo, nullptr, &images[i].imageView) != VK_SUCCESS)
				Debug::Logger::LogFatal("Blaze Engine", "Failed to create image views!\n");

			VkImageView attachments[] = { images[i].imageView };

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = extent.x;
			framebufferInfo.height = extent.y;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(graphicsContext.GetDevice(), &framebufferInfo, nullptr, &images[i].framebuffer) != VK_SUCCESS)
				Debug::Logger::LogFatal("Blaze Engine", "Failed to create framebuffer!\n");
		}

		vkDestroyRenderPass(graphicsContext.GetDevice(), renderPass, nullptr);
	}
}