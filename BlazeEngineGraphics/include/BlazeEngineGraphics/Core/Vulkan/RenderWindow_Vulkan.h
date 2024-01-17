#pragma once
#include "vulkan/vulkan.h"
#include "RenderWindowFramebuffer_Vulkan.h"
#include "Swapchain_Vulkan.h"

namespace Blaze::Graphics::Vulkan
{
	class GraphicsContext_Vulkan;
	class RenderWindowFramebuffer_Vulkan;
	class Semaphore_Vulkan;

	class BLAZE_GRAPHICS_API RenderWindow_Vulkan :
		private EventHandler<Input::Events::WindowResizedEvent>,
		private EventHandler<Input::Events::WindowMinimizedEvent>
	{
	public:
		/*Parity*/RenderWindow_Vulkan(GraphicsContext_Vulkan& graphicsContext, StringView title, Vec2i pos, Vec2u size);
		/*Parity*/~RenderWindow_Vulkan(); 
		/*Parity*/bool AcquireNextFramebuffer(Semaphore_Vulkan& signalSemaphore, RenderWindowFramebuffer_Vulkan*& framebuffer);
		/*Parity*/void PresentFramebuffer(RenderWindowFramebuffer_Vulkan& framebuffer, Array<Semaphore_Vulkan*> semaphores);		

		/*Parity*/inline GraphicsContext_Vulkan& GetGraphicsContext() const { return graphicsContext; }
		/*Parity*/inline Vec2u GetSize() const { return size; }
		/*Parity*/inline WindowSDL& GetWindow() { return window; }

		inline VkSurfaceKHR GetSurface() const { return surface; }		

		inline VkSurfaceFormatKHR GetSurfaceFormat() const { return surfaceFormat; }
		inline VkPresentModeKHR GetPresentMode() const { return presentMode; }
		inline VkExtent2D GetExtent() const { return extent; }
		inline VkSurfaceTransformFlagBitsKHR GetSwapchainTransform() const { return swapchainTransform; }
		inline VkSharingMode GetSwapchainsSharingMode() const { return swapchainsSharingMode; }
		inline ArrayView<uint> GetSwapchainsQueueFamilies() const { return swapchainsQueueFamilies; }
		inline uintMem GetImageCount() const { return imageCount; }
	private:
		GraphicsContext_Vulkan& graphicsContext;

		VkSurfaceKHR surface;
		WindowSDL window;

		class FramebufferData
		{
		public:
			FramebufferData(Swapchain_Vulkan& swapchain, uint imageIndex);
			~FramebufferData();

			inline VkFence GetFence() { return fence; }
			inline RenderWindowFramebuffer_Vulkan& GetFramebuffer() { return framebuffer; }
		private:
			RenderWindowFramebuffer_Vulkan framebuffer;
			VkFence fence;
		};
		class SwapchainResources
		{
		public:
			SwapchainResources(RenderWindow_Vulkan& renderWindow, Swapchain_Vulkan* oldSwapchain);
			~SwapchainResources();

			bool UpdateRetired();

			inline FramebufferData& GetFramebufferData(uint index) { return framebuffersData[index]; }
			inline Swapchain_Vulkan& GetSwapchain() { return swapchain; }
		private:
			uint retiredFrames;
			Swapchain_Vulkan swapchain;
			Array<FramebufferData> framebuffersData;
		};

		struct SwapchainSupportDetails {
			VkSurfaceCapabilitiesKHR capabilities;
			Array<VkSurfaceFormatKHR> formats;
			Array<VkPresentModeKHR> presentModes;
		};

		List<SwapchainResources*> retiredResources;
		SwapchainResources* activeResources;

		Vec2u size;
		bool newSwapchainRequired;
		uint framebufferIndex;
		
		SwapchainSupportDetails swapchainSupportDetails;
		VkSurfaceFormatKHR surfaceFormat;
		VkPresentModeKHR presentMode;
		VkSharingMode swapchainsSharingMode;
		VkSurfaceTransformFlagBitsKHR swapchainTransform;
		Array<uint> swapchainsQueueFamilies;
		VkExtent2D extent;
		uintMem imageCount;

		void QuerrySwapchainSupportDetails();
		void SetSwapchainsProperties();
		
		void CreateNewSwapchain();		

		void CheckRetiredResources();
		void OnEvent(Input::Events::WindowResizedEvent event) override;
		void OnEvent(Input::Events::WindowMinimizedEvent event) override;

		friend class RenderWindowFramebuffer_Vulkan;		
	};
}