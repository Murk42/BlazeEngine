#pragma once
#include "../../Framebuffer.h"
#include "Vulkan_RenderContext.h"
#include "vulkan/vulkan.h"

namespace Graphics2
{
	class Vulkan_RenderContext;

	class Vulkan_Framebuffer : public Framebuffer
	{	
	public:
		using ContextType = Vulkan_RenderContext;

		Vulkan_RenderContext* renderContext;
		VkFramebuffer framebuffer;
		
		Vulkan_Framebuffer(Vulkan_RenderContext* renderContext);
		Vulkan_Framebuffer(VkFramebuffer handle);
		~Vulkan_Framebuffer() override;
	};
}