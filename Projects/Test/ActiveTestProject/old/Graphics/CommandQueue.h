#pragma once
#include "RenderPass.h"

namespace Experimental::Graphics
{
	class RenderContext;

	class CommandBuffer;
	class Swapchain;
	class Framebuffer;
	class Semaphore;
	class Fence;

	enum class CommandQueueType
	{
		Render, Present
	};	

	struct CommandQueueWait
	{
		PipelineStage stage;
		Semaphore* semaphore;
	};

	class CommandQueue
	{
	public:
		CommandQueue(RenderContext& renderContext, CommandQueueType type);
		~CommandQueue();

		void Submit(ArrayView<CommandBuffer*> commandBuffers, ArrayView<CommandQueueWait> waitSemaphores, ArrayView<Semaphore*> signalSemaphore, Fence& fence);
		void SubmitPresentCommand(Swapchain& swapbuffer, Framebuffer& framebuffer, Array<Semaphore*> waitSemaphores);

		inline void* GetHandle() const { return impl; }
	private:
		void* impl;
	};
}