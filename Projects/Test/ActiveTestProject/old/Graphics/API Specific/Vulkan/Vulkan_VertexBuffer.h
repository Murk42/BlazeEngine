#pragma once
#include "vulkan/vulkan.h"
#include "Vulkan_RenderContext.h"

namespace Experimental::Graphics::Vulkan
{
	struct CommandBufferPoolImpl;
	struct CommandQueueImpl;

	struct VertexBufferImpl
	{
		RenderContextImpl* renderContext;

		VkDeviceMemory memory;
		VkBuffer vertexBuffer;

		VertexBufferImpl(RenderContextImpl* renderContext, const void* data, uint size, CommandBufferPoolImpl* commandPool, CommandQueueImpl* queue);
		~VertexBufferImpl();
	};
}