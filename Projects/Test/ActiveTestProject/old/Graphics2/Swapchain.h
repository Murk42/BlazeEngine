#pragma once
#include "CustomRenderBackend.h"

namespace Graphics2
{	
	class Semaphore;
	class Framebuffer;
	class RenderWindow;

	class Swapchain : public CustomRenderBackend<Swapchain>
	{
	public:
		virtual ~Swapchain();
		
		virtual void Create(RenderWindow*) = 0;
		virtual void Destroy() = 0;

		virtual Framebuffer& AdvanceFramebuffers(Semaphore*) = 0;
	};
}