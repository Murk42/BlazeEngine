#pragma once
#include "vulkan/vulkan.h"
#include "Vulkan_RenderContext.h"
#include "../../Semaphore.h"

namespace Experimental::Graphics::Vulkan
{
	struct SemaphoreImpl
	{
		RenderContextImpl* renderContext;
		VkSemaphore semaphore;

		SemaphoreImpl(RenderContextImpl* renderContext);
		~SemaphoreImpl();		
	};
}