#pragma once
#include "Framebuffer_Vulkan.h"

namespace Blaze::Graphics::Vulkan
{
	class Framebuffer_Vulkan;
	class RenderWindow_Vulkan;
	class Swapchain_Vulkan;

	class BLAZE_GRAPHICS_API RenderWindowFramebuffer_Vulkan
	{
	public:
		/*Parity*/~RenderWindowFramebuffer_Vulkan();

		/*Parity*/inline Framebuffer_Vulkan& GetFramebuffer() { return framebuffer; }
		/*Parity*/RenderWindow_Vulkan& GetRenderWindow();
		
		RenderWindowFramebuffer_Vulkan(Swapchain_Vulkan& swapchain, uint imageIndex);

		void SetImageIndex(uint newImageIndex);

		inline Swapchain_Vulkan& GetSwapchain() { return swapchain; }
		inline uint GetImageIndex() const { return imageIndex; };
	private:
		Swapchain_Vulkan& swapchain;
		uint imageIndex;
		Framebuffer_Vulkan framebuffer;		
	};
}