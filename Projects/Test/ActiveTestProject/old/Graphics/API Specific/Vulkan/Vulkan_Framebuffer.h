#pragma once
#include "vulkan/vulkan.h"
#include "Vulkan_RenderContext.h"

namespace Experimental::Graphics::Vulkan
{
	struct FramebufferImpl
	{
		RenderContextImpl* renderContext;
		VkFramebuffer framebuffer;
		
		FramebufferImpl();
		FramebufferImpl(VkFramebuffer handle);
		~FramebufferImpl();
	};
}