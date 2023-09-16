#include "CommandQueue.h"
#include "API Specific/Vulkan/Vulkan_CommandQueue.h"
#include "RenderContext.h"
#include "Fence.h"
#include "Swapchain.h"

namespace Experimental::Graphics
{
	using namespace Vulkan;

	CommandQueue::CommandQueue(RenderContext& renderContext, CommandQueueType type)
	{
		impl = new CommandQueueImpl((RenderContextImpl*)renderContext.GetHandle(), type);
	}
	CommandQueue::~CommandQueue()
	{
		delete (CommandQueueImpl*)impl;
	}
	void CommandQueue::Submit(ArrayView<CommandBuffer*> commandBuffers, ArrayView<CommandQueueWait> waitSemaphores, ArrayView<Semaphore*> signalSemaphore, Fence& fence)
	{
		((CommandQueueImpl*)impl)->Submit(commandBuffers, waitSemaphores, signalSemaphore, (FenceImpl*)fence.GetHandle());
	}
	void CommandQueue::SubmitPresentCommand(Swapchain& swapbuffer, Framebuffer& framebuffer, Array<Semaphore*> waitSemaphores)
	{
		((CommandQueueImpl*)impl)->SubmitPresentCommand((SwapchainImpl*)swapbuffer.GetHandle(), &framebuffer, waitSemaphores);
	}
}
