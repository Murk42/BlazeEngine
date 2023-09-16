#include "RenderBackends.h"
#include "Backends/Vulkan/Vulkan_Fence.h"
#include "Backends/Vulkan/Vulkan_Framebuffer.h" 
#include "Backends/Vulkan/Vulkan_RenderContext.h"
#include "Backends/Vulkan/Vulkan_Semaphore.h"
#include "Backends/Vulkan/Vulkan_Swapchain.h"
#include "Backends/Vulkan/Vulkan_RenderWindow.h"

namespace Graphics2
{
	extern uint64 renderBackendID_OpenGL;
	extern uint64 renderBackendID_Vulkan;

	void AddDefaultBackends()
	{
		renderBackendID_Vulkan = RenderContext::AddCustomBackend<Vulkan_RenderContext>();
		Fence::SetCustomBackend<Vulkan_Fence>(renderBackendID_Vulkan);
		Framebuffer::SetCustomBackend<Vulkan_Framebuffer>(renderBackendID_Vulkan);
		Semaphore::SetCustomBackend<Vulkan_Semaphore>(renderBackendID_Vulkan);
		Swapchain::SetCustomBackend<Vulkan_Swapchain>(renderBackendID_Vulkan);
		RenderWindow::SetCustomBackend<Vulkan_RenderWindow>(renderBackendID_Vulkan);
	}
}