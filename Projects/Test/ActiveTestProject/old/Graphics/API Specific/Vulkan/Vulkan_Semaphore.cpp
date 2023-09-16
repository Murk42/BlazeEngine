#include "Vulkan_Semaphore.h"
#include "Vulkan_Instance.h"
#include <iostream>

namespace Experimental::Graphics::Vulkan
{
	SemaphoreImpl::SemaphoreImpl(RenderContextImpl* renderContext)
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
	SemaphoreImpl::~SemaphoreImpl()
	{
		vkDestroySemaphore(renderContext->device, semaphore, nullptr);
	}	
}