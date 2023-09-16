#pragma once
#include "vulkan/vulkan.h"
#include "Vulkan_RenderContext.h"
#include "Vulkan_Instance.h"

namespace Experimental::Graphics::Vulkan
{
	struct RenderContextImpl
	{
		VkDevice device;
		VkPhysicalDevice physicalDevice;

		uint32_t graphicsQueueFamily;
		uint32_t presentQueueFamily;

		uint graphicsQueueCount;
		uint presentQueueCount;

		RenderContextImpl();
		~RenderContextImpl();
	};
}