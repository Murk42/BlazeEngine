#pragma once
#include "vulkan/vulkan.h"

namespace Blaze::Graphics::Vulkan
{
	class GraphicsContext_Vulkan;

	class BLAZE_GRAPHICS_API Semaphore_Vulkan
	{
	public:
		/*Parity*/Semaphore_Vulkan(GraphicsContext_Vulkan&);
		/*Parity*/~Semaphore_Vulkan();

		inline VkSemaphore GetHandle() const { return handle; }
	private:
		VkSemaphore handle;
		GraphicsContext_Vulkan& graphicsContext;
	};
}