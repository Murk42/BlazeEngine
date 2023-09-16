#pragma once
#include "vulkan/vulkan.h"
#include "Vulkan_RenderContext.h"
#include "../../Semaphore.h"

namespace Graphics2
{
	class Vulkan_Semaphore : public Semaphore
	{
	public:
		using ContextType = Vulkan_RenderContext;

		Vulkan_RenderContext* renderContext;
		VkSemaphore semaphore;

		Vulkan_Semaphore(Vulkan_RenderContext* renderContext);
		~Vulkan_Semaphore() override;
	};
}