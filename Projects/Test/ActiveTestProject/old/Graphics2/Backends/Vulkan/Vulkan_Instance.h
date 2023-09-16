#pragma once
#include "vulkan/vulkan.h"
#include "SDL/SDL_vulkan.h"
#include "Vulkan_RenderWindow.h"

namespace Graphics2
{
#ifdef _DEBUG
	const Array<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
	};
#endif

	struct VulkanInstance
	{			
		VkInstance instance = VK_NULL_HANDLE;		

		SDL_Window* initRenderWindow;
		VkSurfaceKHR surface;		
		bool windowTaken;

		uint contextCount = 0;

#ifdef _DEBUG
		VkDebugUtilsMessengerEXT debugMessenger;
#endif		
	};

	extern VulkanInstance vulkanInstance;

	void CreateVulkanInstance(bool windowSupport);	
	void FreeVulkanInstance();
}