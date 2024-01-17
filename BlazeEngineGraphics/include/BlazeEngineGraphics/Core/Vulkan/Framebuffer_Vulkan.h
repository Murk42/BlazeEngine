#pragma once
#include "vulkan/vulkan.h"

namespace Blaze::Graphics::Vulkan
{
	class RenderWindowFramebuffer_Vulkan;

	class BLAZE_GRAPHICS_API Framebuffer_Vulkan
	{
	public:
		/*Parity*/~Framebuffer_Vulkan();

		/*Parity*/inline Vec2u GetSize() const { return size; }
		/*Parity*/inline RenderWindowFramebuffer_Vulkan* GetRenderWindowFramebuffer() { return renderWindowFramebuffer; }

		Framebuffer_Vulkan(VkImage image, VkFramebuffer handle, Vec2u size, RenderWindowFramebuffer_Vulkan* renderWindowFramebuffer);		

		inline VkFramebuffer GetHandle() const { return handle; };
		inline VkImage GetImage() const { return image; }		
	private:
		VkImage image;
		VkFramebuffer handle;
		Vec2u size;		
		RenderWindowFramebuffer_Vulkan* renderWindowFramebuffer;

		friend class RenderWindowFramebuffer_Vulkan;
	};
}