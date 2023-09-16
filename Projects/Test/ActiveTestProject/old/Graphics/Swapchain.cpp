#include "Swapchain.h"
#include "API Specific/Vulkan/Vulkan_Swapchain.h"
#include "API Specific/Vulkan/Vulkan_Framebuffer.h"
#include "RenderContext.h"
#include "Semaphore.h"
#include "Framebuffer.h"

namespace Experimental::Graphics
{
	using namespace Vulkan;

	Swapchain::Swapchain(RenderContext& renderContext, Window& window)
	{
		this->impl = new SwapchainImpl((RenderContextImpl*)renderContext.GetHandle(), (SDL_Window*)window.GetHandle());
	}
	Swapchain::~Swapchain()
	{		
		delete (SwapchainImpl*)impl;
	}
	Framebuffer& Swapchain::GetNextFramebuffer(Semaphore& signalSemaphore)
	{
		return ((SwapchainImpl*)impl)->GetNextFramebuffer((SemaphoreImpl*)signalSemaphore.GetHandle());		
	}
}
