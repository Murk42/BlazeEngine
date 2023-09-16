#pragma once
#include "../../RenderWindow.h"
#include "vulkan/vulkan.h"
#include "SDL/SDL.h"

namespace Graphics2
{
	class Vulkan_RenderContext;

	class Vulkan_RenderWindow : public RenderWindow
	{
	public:
		using ContextType = Vulkan_RenderContext;

		Vulkan_RenderContext* renderContext;
		Handle<Window> engineWindow;
		SDL_Window* window;
		VkSurfaceKHR surface;

		Vulkan_RenderWindow(Vulkan_RenderContext* renderContext);
		Vulkan_RenderWindow(Vulkan_RenderWindow&& renderWindow);
		~Vulkan_RenderWindow() override;		

		Window& GetWindow() override;
		void Present(Framebuffer*, Swapchain*, Semaphore* waitSemaphore) override;

		Vulkan_RenderWindow& operator=(Vulkan_RenderWindow&&) noexcept;
	};
}