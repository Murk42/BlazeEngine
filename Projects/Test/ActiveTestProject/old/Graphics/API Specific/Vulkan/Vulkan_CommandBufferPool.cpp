#include "Vulkan_CommandBufferPool.h"
#include <iostream>

namespace Experimental::Graphics::Vulkan
{
	CommandBufferPoolImpl::CommandBufferPoolImpl(RenderContextImpl* renderContext, CommandQueueType type)
		: renderContext(renderContext)
	{
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;        

		uint32 queueFamilyIndex;
		switch (type)
		{
		case Experimental::Graphics::CommandQueueType::Render: queueFamilyIndex = renderContext->graphicsQueueFamily; break;
		case Experimental::Graphics::CommandQueueType::Present: queueFamilyIndex = renderContext->presentQueueFamily;  break;
		}
        poolInfo.queueFamilyIndex = queueFamilyIndex;

        if (vkCreateCommandPool(renderContext->device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
        {
            std::cout << "failed to create command pool!\n";
            std::exit(1);
        }
	}
	CommandBufferPoolImpl::~CommandBufferPoolImpl()
	{
		vkDestroyCommandPool(renderContext->device, commandPool, nullptr);
	}
}