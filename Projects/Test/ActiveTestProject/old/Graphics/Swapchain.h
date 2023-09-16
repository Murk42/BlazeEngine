#pragma once

namespace Experimental::Graphics
{
	class RenderContext;
	class Framebuffer;
	class Semaphore;

	class Swapchain
	{
	public:
		Swapchain(RenderContext& renderContext, Window& window);
		~Swapchain();

		Framebuffer& GetNextFramebuffer(Semaphore& signalSemaphore);

		inline void* GetHandle() const { return impl; }
	private:
		void* impl;				
	};
}
