#pragma once
#include "vulkan/vulkan.h"
#include "Vulkan_RenderContext.h"
#include "../../CommandQueue.h"

namespace Experimental::Graphics::Vulkan
{
	struct CommandBufferPoolImpl
	{
		RenderContextImpl* renderContext;
		
		VkCommandPool commandPool;

		CommandBufferPoolImpl(RenderContextImpl* renderContext, CommandQueueType type);
		~CommandBufferPoolImpl();
	};
}