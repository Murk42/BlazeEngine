#pragma once
#include "../../RenderContext.h"
#include "vulkan/vulkan.h"

namespace Graphics2
{
	class Vulkan_RenderContext : public RenderContext
	{
	public:
		VkDevice device;
		VkPhysicalDevice physicalDevice;

		uint32 graphicsQueueFamily;
		uint32 presentQueueFamily;

		VkQueue presentQueue;

		Vulkan_RenderContext();
		~Vulkan_RenderContext() override;		
	};
}