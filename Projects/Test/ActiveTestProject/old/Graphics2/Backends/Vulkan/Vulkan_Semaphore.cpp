#include "Vulkan_Semaphore.h"
#include "Vulkan_Instance.h"
#include <iostream>

namespace Graphics2
{
	Vulkan_Semaphore::Vulkan_Semaphore(Vulkan_RenderContext* renderContext)
		: renderContext(renderContext)
	{
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		if (vkCreateSemaphore(renderContext->device, &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS)
		{
			std::cout << "Failed to create fence\n";
			std::exit(1);
		}
	}
	Vulkan_Semaphore::~Vulkan_Semaphore()
	{
		vkDestroySemaphore(renderContext->device, semaphore, nullptr);
	}
}
