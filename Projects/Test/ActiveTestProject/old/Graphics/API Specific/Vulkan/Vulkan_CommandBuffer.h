#pragma once
#include "vulkan/vulkan.h"
#include "Vulkan_RenderContext.h"
#include "../../CommandQueue.h"

namespace Experimental::Graphics::Vulkan
{
	struct CommandBufferPoolImpl;
	struct RenderPassImpl;
	struct FramebufferImpl;

	struct CommandBufferImpl
	{		
		RenderContextImpl* renderContext;

		VkCommandBuffer commandBuffer;

		CommandBufferImpl(CommandBufferPoolImpl* commandPool);
		~CommandBufferImpl();

		void Record(RenderPassImpl* renderPass, FramebufferImpl* framebuffer, Recti rect, ArrayView<Pipelinex);
		void Reset();
	};
}