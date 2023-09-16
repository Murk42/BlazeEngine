#include "Vulkan_RenderWindow.h"
#include "Vulkan_Framebuffer.h"
#include "Vulkan_Swapchain.h"
#include "Vulkan_Instance.h"
#include "SDL/SDL.h"
#include <iostream>

namespace Graphics2
{    
    Vulkan_RenderWindow::Vulkan_RenderWindow(Vulkan_RenderContext* renderContext)
        : renderContext(renderContext)
	{
        if (!vulkanInstance.windowTaken)
        {
            window = vulkanInstance.initRenderWindow;
            surface = vulkanInstance.surface;            
            return;
        }

        window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 420,
            SDL_WINDOW_VULKAN | SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE);
        
        if (window == nullptr)
        {
            std::cout << "SDL_CreateWindow failed: \"" << SDL_GetError() << "\"\n";
            std::exit(1);
        }     
        
        if (SDL_Vulkan_CreateSurface(window, vulkanInstance.instance, &surface) == SDL_FALSE)
        {
            std::cout << "SDL failed to create vulkan surface: \"" << SDL_GetError() << "\"\n";
            std::exit(1);
        }
        
        engineWindow = Handle<Window>::CreateNew((void*)window, false);
	}
    Vulkan_RenderWindow::Vulkan_RenderWindow(Vulkan_RenderWindow&& renderWindow)
        : renderContext(renderWindow.renderContext), window(renderWindow.window), surface(renderWindow.surface)
    {
        renderWindow.renderContext = nullptr;
        renderWindow.window = nullptr;
        renderWindow.surface = nullptr;
    }
	Vulkan_RenderWindow::~Vulkan_RenderWindow()
	{
        if (surface != vulkanInstance.surface)
        {
            vkDestroySurfaceKHR(vulkanInstance.instance, surface, nullptr);
            SDL_DestroyWindow(window);
        }
	}
    Window& Vulkan_RenderWindow::GetWindow()
    {
        return *engineWindow;
    }
    void Vulkan_RenderWindow::Present(Framebuffer* framebuffer, Swapchain* swapchain, Semaphore* waitSemaphore)
    {
        Vulkan_Swapchain* vulkanSwapchain = (Vulkan_Swapchain*)swapchain;
        Vulkan_Semaphore* vulkanSemaphore = (Vulkan_Semaphore*)waitSemaphore;
        Vulkan_Framebuffer* vulkanFramebuffer = (Vulkan_Framebuffer*)framebuffer;

        uint32 imageIndex = vulkanFramebuffer - vulkanSwapchain->framebuffers.Ptr();
        VkPresentInfoKHR presentInfo{ };
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pResults = nullptr;
        presentInfo.pSwapchains = &vulkanSwapchain->swapChain;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.swapchainCount = 1;
        presentInfo.pWaitSemaphores = &vulkanSemaphore->semaphore;
        presentInfo.waitSemaphoreCount = 1;

        vkQueuePresentKHR(renderContext->presentQueue, &presentInfo);
    }
    Vulkan_RenderWindow& Vulkan_RenderWindow::operator=(Vulkan_RenderWindow&& other) noexcept
    {
        renderContext = other.renderContext;
        window = other.window;
        surface = other.surface;
        other.renderContext = nullptr;
        other.window = nullptr;
        other.surface = nullptr;

        return *this;
    }
}