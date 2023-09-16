#pragma once
#include "vulkan/vulkan.h"
#include "SDL/SDL_vulkan.h"

namespace Experimental::Graphics::Vulkan
{
#ifdef _DEBUG
	const Array<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
	};
#endif

	struct VulkanInstance
	{			
		VkInstance instance;

		SDL_Window* initRenderWindow;
		VkSurfaceKHR surface;		
		bool surfaceTaken;

#ifdef _DEBUG
		VkDebugUtilsMessengerEXT debugMessenger;
#endif		
	};

	extern VulkanInstance vulkanInstance;

	void CreateVulkanInstance(bool windowSupport);	
	void FreeVulkanInstance();
}