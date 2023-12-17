#pragma once
#include "vulkan/vulkan.h"

namespace Blaze::Graphics::Vulkan
{
	class GraphicsContext_Vulkan;
	class RenderWindow_Vulkan;

	class BLAZE_GRAPHICS_API Swapchain_Vulkan
	{
	public:
		Swapchain_Vulkan(RenderWindow_Vulkan& renderWindow, Swapchain_Vulkan* old);		
		~Swapchain_Vulkan();				

		inline GraphicsContext_Vulkan& GetGraphicsContext() { return graphicsContext; }
		inline RenderWindow_Vulkan& GetRenderWindow() { return renderWindow; }
		inline VkSwapchainKHR GetHandle() const { return swapchain; }
		inline bool IsRetired() const { return retired; }
		inline Vec2u GetExtent() const { return extent; }
		inline uint GetImageCount() const { return images.Count(); }
		inline VkFramebuffer GetFramebuffer(uint index) const { return images[index].framebuffer; }		
		inline VkImage GetImage(uint index) const { return images[index].image; }
	private:		
		GraphicsContext_Vulkan& graphicsContext;
		RenderWindow_Vulkan& renderWindow;
		VkSwapchainKHR swapchain;

		VkFormat format;
		Vec2u extent;

		struct ImageData
		{
			VkImage image;
			VkImageView imageView;
			VkFramebuffer framebuffer;
		};
		Array<ImageData> images;

		bool retired;
		
		void CreateResources();
	};
}