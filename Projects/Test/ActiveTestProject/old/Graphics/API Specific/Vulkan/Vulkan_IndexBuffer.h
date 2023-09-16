#pragma once
#include "vulkan/vulkan.h"
#include "Vulkan_RenderContext.h"

namespace Experimental::Graphics::Vulkan
{
	struct CommandBufferPoolImpl;
	struct CommandQueueImpl;

	struct IndexBufferImpl
	{
		RenderContextImpl* renderContext;

		VkDeviceMemory memory;
		VkBuffer indexBuffer;

		IndexBufferImpl(RenderContextImpl* renderContext, const void* data, uint size, CommandBufferPoolImpl* commandPool, CommandQueueImpl* queue);
		~IndexBufferImpl();
	};
}