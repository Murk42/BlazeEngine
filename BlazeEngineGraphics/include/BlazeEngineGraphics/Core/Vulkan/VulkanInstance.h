#pragma once
#include "vulkan/vulkan.h"
#include "SDL3/SDL_vulkan.h"

#ifdef _DEBUG
#define BLAZE_VULKAN_VALIDATION_LAYERS
#endif

namespace Blaze::Graphics::Vulkan
{
#ifdef BLAZE_VULKAN_VALIDATION_LAYERS
	const Array<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
	};
#endif

	class BLAZE_GRAPHICS_API VulkanInstance
	{		
	public:
		VulkanInstance();		
		VulkanInstance(const VulkanInstance&) = delete;
		VulkanInstance(VulkanInstance&&) noexcept;
		~VulkanInstance();

		void Destroy();

		Window CreateWindow(StringView title, Vec2i pos, Vec2u size);
		void FreeWindow(Window& window);

		inline VkInstance GetHandle() { return instance; }		

		VulkanInstance& operator=(const VulkanInstance&) = delete;
		VulkanInstance& operator=(VulkanInstance&&) noexcept;
	private:
		VkInstance instance;

		Window initWindow;
		
#ifdef BLAZE_VULKAN_VALIDATION_LAYERS
		VkDebugUtilsMessengerEXT debugMessenger;
#endif		
	};
}