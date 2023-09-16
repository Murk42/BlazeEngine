#pragma once
#include "vulkan/vulkan.h"
#include "Vulkan_RenderContext.h"
#include "../../CommandQueue.h"

namespace Experimental::Graphics::Vulkan
{
	struct FenceImpl;
	struct SwapchainImpl;
	struct FramebufferImpl;

	struct CommandQueueImpl
	{
		RenderContextImpl* renderContext;
		VkQueue queue;

		CommandQueueImpl(RenderContextImpl* renderContext, CommandQueueType type);
		~CommandQueueImpl();

		void Submit(ArrayView<CommandBuffer*> commandBuffers, ArrayView<CommandQueueWait> waitSemaphores, ArrayView<Semaphore*> signalSemaphore, FenceImpl* fence);
		void SubmitPresentCommand(SwapchainImpl* swapbuffer, Framebuffer* framebuffer, Array<Semaphore*> waitSemaphores);
	};
}