#include "pch.h"
#include "BlazeEngineGraphics\Vulkan/Core/GraphicsContext_Vulkan.h"
#include "BlazeEngineGraphics\Vulkan/Core/Semaphore_Vulkan.h"

namespace Blaze::Graphics::Vulkan
{
	Semaphore_Vulkan::Semaphore_Vulkan(GraphicsContext_Vulkan& graphicsContext)
		: handle(nullptr), graphicsContext(graphicsContext)
	{		
		VkSemaphoreCreateInfo createInfo{ };
		createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		vkCreateSemaphore(graphicsContext.GetDevice(), &createInfo, nullptr, &handle);
	}
	Semaphore_Vulkan::~Semaphore_Vulkan()
	{
		vkDestroySemaphore(graphicsContext.GetDevice(), handle, nullptr);
	}

}