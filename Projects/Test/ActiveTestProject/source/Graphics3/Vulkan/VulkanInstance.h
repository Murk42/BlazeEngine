#pragma once
#include "vulkan/vulkan.h"
#include "SDL/SDL_vulkan.h"

#ifdef _DEBUG
#define BLAZE_VULKAN_VALIDATION_LAYERS
#endif

namespace Graphics3::Vulkan
{
#ifdef BLAZE_VULKAN_VALIDATION_LAYERS
	const Array<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
	};
#endif

	class VulkanInstance
	{		
	public:
		VulkanInstance();		
		VulkanInstance(VulkanInstance&&) noexcept;
		~VulkanInstance();

		SDL_Window* CreateWindow();
		void FreeWindow(SDL_Window* window);

		inline VkInstance GetHandle() { return instance; }		

		VulkanInstance& operator=(const VulkanInstance&);
	private:
		VkInstance instance;

		SDL_Window* initWindow;			
		
#ifdef BLAZE_VULKAN_VALIDATION_LAYERS
		VkDebugUtilsMessengerEXT debugMessenger;
#endif		
	};
}