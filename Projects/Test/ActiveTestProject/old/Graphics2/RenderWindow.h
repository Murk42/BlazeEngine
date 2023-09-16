#pragma once
#include "CustomRenderBackend.h"

namespace Graphics2
{
	class Framebuffer;
	class Swapchain;
	class Semaphore;

	class RenderWindow : public CustomRenderBackend<RenderWindow>
	{
	public:
		virtual ~RenderWindow();

		virtual Window& GetWindow() = 0;
		virtual void Present(Framebuffer* framebuffer, Swapchain* swapchain, Semaphore* waitSemaphore) = 0;
	};
}