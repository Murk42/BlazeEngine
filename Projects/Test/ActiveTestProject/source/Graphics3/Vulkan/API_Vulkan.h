#pragma once
#include "../API/API_dynamic.h"
#include "vulkan/vulkan.h"
#include "SDL/SDL.h"
#include "SDL/SDL_vulkan.h"
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include "vk_mem_alloc.h"

namespace Graphics3
{
	namespace Vulkan
	{
		class GraphicsContext_Vulkan;
		class Semaphore_Vulkan;
		class RenderContext_Vulkan;
		class Framebuffer_Vulkan;
		class Swapchain_Vulkan;
		class RenderWindowFramebuffer_Vulkan;
		class RenderWindow_Vulkan;

		using GraphicsContext = GraphicsContext_Vulkan;
		using Semaphore = Semaphore_Vulkan;
		using RenderContext = RenderContext_Vulkan;
		using Framebuffer = Framebuffer_Vulkan;
		using Swapchain = Swapchain_Vulkan;
		using RenderWindowFramebuffer = RenderWindowFramebuffer_Vulkan;
		using RenderWindow = RenderWindow_Vulkan;

		class BlazeVMAData
		{
		public:
			BlazeVMAData(GraphicsContext& graphicsContext);
			BlazeVMAData(BlazeVMAData&& other) noexcept;
			~BlazeVMAData();

			inline VmaAllocator GetAllocator() const { return allocator; }

			void Clear();

			BlazeVMAData& operator=(BlazeVMAData&& other) noexcept;
		private:
			VmaAllocator allocator;
		};		

		BlazeVMAData* GetBlazeVMAData(GraphicsContext& graphicsContext);		

		class GraphicsContext_Vulkan : public Dynamic::GraphicsContextBase
		{
		public:
			VirtualMap<String> dataMap;

			GraphicsContext_Vulkan(Set<String>&& extensions);
			GraphicsContext_Vulkan(GraphicsContext_Vulkan&&) noexcept;
			~GraphicsContext_Vulkan() override;

			void WaitForIdle() const override;

			void Clear() override;

			void CreateWindowAndSurface(SDL_Window*& window, VkSurfaceKHR& surface);
			void DestroyWindowAndSurface(SDL_Window* window, VkSurfaceKHR surface);

			void RunSingleTimeCommand(const std::function<void(VkCommandBuffer)>&);
			
			//VkFormat ChooseFormatForLinearTilingImage(std::initializer_list<VkFormat> formats, VkFormatFeatureFlags features);
			//VkFormat ChooseFormatForOptimalTilingImage(std::initializer_list<VkFormat> formats, VkFormatFeatureFlags features);
			//VkFormat ChooseFormatForBuffer(std::initializer_list<VkFormat> formats, VkFormatFeatureFlags features);
			//VkFormat GetFormatForLinearTilingImage(BitmapColorFormat format, BitmapColorComponentType type, VkFormatFeatureFlags flags);
			//VkFormat GetFormatForOptimalTilingImage(BitmapColorFormat format, BitmapColorComponentType type, VkFormatFeatureFlags flags);
			//VkFormat GetFormatForLinearTilingImage(BitmapColorFormat format, BitmapColorComponentType type, VkFormatFeatureFlags flags);

			void* CreateStagingBuffer(uint size, VkBuffer& buffer, VmaAllocation& allocation);
			void CreateImage2D(Vec2i size, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImageLayout layout, VkImage& image, VmaAllocation& allocation);
			void CopyBufferToImage(VkBuffer buffer, VkImage image, uint bufferOffset, uint bufferBitmapStride, Vec2i imageOffset, Vec2i imageSize, VkImageLayout layout);
			void TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage, VkAccessFlags srcAccess, VkAccessFlags dstAccess);

			inline VkDevice GetDevice() const { return device; }
			inline VkPhysicalDevice GetPhysicalDevice() const { return physicalDevice; }
			inline uint32 GetGraphicsQueueFamily() const { return graphicsQueueFamily; }
			inline uint32 GetPresentQueueFamily() const { return presentQueueFamily; }
			inline const Set<String>& GetEnabledExtensions() const { return enabledExtensions; }

			GraphicsContext_Vulkan& operator=(GraphicsContext_Vulkan&&) noexcept;
		private:
			VkDevice device;
			VkPhysicalDevice physicalDevice;
			Set<String> enabledExtensions;
			uint32 graphicsQueueFamily;
			uint32 presentQueueFamily;						

			SDL_Window* initWindow;
			VkSurfaceKHR initSurface;
			bool initWindowTaken;

			VkQueue graphicsQueue;
			VkCommandPool commandPool;
		};

		class Semaphore_Vulkan : public Dynamic::SemaphoreBase
		{
		public:
			Semaphore_Vulkan(GraphicsContext_Vulkan&);
			Semaphore_Vulkan(Semaphore_Vulkan&&) noexcept;
			~Semaphore_Vulkan() override;

			void Clear() override;			

			inline VkSemaphore GetHandle() const { return handle; }

			Semaphore_Vulkan& operator=(Semaphore_Vulkan&&) noexcept;
		private:
			VkSemaphore handle;
			GraphicsContext_Vulkan* graphicsContext;
		};

		class RenderContext_Vulkan : public Dynamic::RenderContextBase
		{
		public:
			RenderContext_Vulkan(GraphicsContext_Vulkan*, VkQueue queue);
			RenderContext_Vulkan(RenderContext_Vulkan&&) noexcept;
			~RenderContext_Vulkan() override;

			void WaitForIdle() const override;

			void Clear() override;

			VkQueue GetQueue() const { return queue; }			

			inline GraphicsContext_Vulkan* GetGraphicsContext() const { return graphicsContext; }

			RenderContext_Vulkan& operator=(RenderContext_Vulkan&&) noexcept;
		private:
			GraphicsContext_Vulkan* graphicsContext;

			VkQueue queue;			
		};

		class Framebuffer_Vulkan : public Dynamic::FramebufferBase
		{
		public:
			Framebuffer_Vulkan();
			Framebuffer_Vulkan(Framebuffer_Vulkan&&) noexcept;
			~Framebuffer_Vulkan() override;

			void Clear() override;

			inline VkFramebuffer GetHandle() const { return handle; };
			inline VkImage GetImage() const { return image; }
			inline Vec2i GetSize() const override { return size; }

			Framebuffer_Vulkan& operator=(Framebuffer_Vulkan&&) noexcept;
		private:
			VkImage image;
			VkFramebuffer handle;
			Vec2i size;

			friend class RenderWindow_Vulkan;
			friend class RenderWindowFramebuffer_Vulkan;
		};		

		class Swapchain_Vulkan
		{
		public:
			GraphicsContext_Vulkan* graphicsContext;
			RenderWindow_Vulkan* renderWindow;
			VkSwapchainKHR swapchain;

			VkFormat format;
			Vec2i size;
			
			struct ImageData
			{
				VkImage image;
				VkImageView imageView;
				VkFramebuffer framebuffer;				
			};
			Array<ImageData> images;
			
			bool retired;

			Swapchain_Vulkan();
			Swapchain_Vulkan(RenderWindow_Vulkan& renderWindow, Swapchain_Vulkan* old);
			Swapchain_Vulkan(Swapchain_Vulkan&&) noexcept;
			~Swapchain_Vulkan();

			void Clear();
			
			Swapchain_Vulkan& operator=(Swapchain_Vulkan&&) noexcept;
		};

		class RenderWindowFramebuffer_Vulkan : public Dynamic::RenderWindowFramebufferBase, public Framebuffer_Vulkan
		{
		public:
			RenderWindowFramebuffer_Vulkan();
			RenderWindowFramebuffer_Vulkan(RenderWindowFramebuffer_Vulkan&&) noexcept;
			~RenderWindowFramebuffer_Vulkan() override;			

			void Clear() override;
			Dynamic::RenderWindowBase* GetRenderWindow() const override;
			Vec2i GetSize() const override;
			operator FramebufferBase&() override;

			RenderWindowFramebuffer_Vulkan& operator=(RenderWindowFramebuffer_Vulkan&&) noexcept;
		private:						
			Swapchain_Vulkan* swapchain;
			uint imageIndex;

			friend class RenderWindow_Vulkan;
		};

		class RenderWindow_Vulkan : public Dynamic::RenderWindowBase, 
			private EventHandler<Input::Events::WindowResizedEvent>,
			private EventHandler<Input::Events::WindowMinimizedEvent>
		{
		public:			
			RenderWindow_Vulkan(GraphicsContext_Vulkan* graphicsContext, String title, Vec2i size);
			RenderWindow_Vulkan(RenderWindow_Vulkan&&) noexcept;
			~RenderWindow_Vulkan() override;

			void Clear() override;

			inline GraphicsContext_Vulkan* GetGraphicsContext() const { return graphicsContext; }			
			inline VkSurfaceKHR GetSurface() const { return surface; }
			inline Window* GetWindow() const override { return engineWindow; }
			inline Vec2i GetSize() const override { return size; }

			RenderWindowFramebuffer_Vulkan& AquireNextFramebuffer(Semaphore_Vulkan& signalSemaphore);
			void PresentRenderWindow(RenderContext_Vulkan& renderContext, RenderWindowFramebuffer_Vulkan& framebuffer, std::initializer_list<Semaphore_Vulkan*> waitSemaphores);

			RenderWindow_Vulkan& operator=(RenderWindow_Vulkan&&) noexcept;			
		private:					
			GraphicsContext_Vulkan* graphicsContext;

			VkSurfaceKHR surface;
			SDL_Window* window;	
			Window* engineWindow;  					

			struct FramebufferData
			{
				RenderWindowFramebuffer_Vulkan framebuffer;
				VkFence fence;
			};
			struct SwapchainResources
			{				
				uint retiredFrames;
				Swapchain_Vulkan swapchain;
				Array<FramebufferData> framebuffersData;
			};

			List<SwapchainResources> retiredResources;
			SwapchainResources activeResources;
						
			Vec2i size;
			bool newSwapchainRequired;
			uint framebufferIndex;

			void CreateSwapchain();
			void RecreateSwapchain();			

			void CreateSwapchainResources(SwapchainResources& resources);

			void CheckRetiredResources();
			void OnEvent(Input::Events::WindowResizedEvent event) override;
			void OnEvent(Input::Events::WindowMinimizedEvent event) override;
		};

		GraphicsContext_Vulkan CreateGraphicsContext(ImplementationInfo* implementationInfo);
		Semaphore_Vulkan CreateSemaphore(GraphicsContext_Vulkan& graphicsContext);
		RenderContext_Vulkan CreateGraphicsRenderContext(GraphicsContext_Vulkan& graphicsContext);
		RenderContext_Vulkan CreatePresentRenderContext(GraphicsContext_Vulkan& graphicsContext);
		RenderWindow_Vulkan CreateRenderWindow(GraphicsContext_Vulkan& graphicsContext, String title, Vec2i size);
		RenderWindowFramebuffer_Vulkan& AquireNextFramebuffer(RenderWindow_Vulkan& renderWindow, Semaphore_Vulkan& signalSemaphore);
		void PresentRenderWindow(RenderContext_Vulkan& renderContext, RenderWindow_Vulkan& renderWindow, RenderWindowFramebuffer_Vulkan& framebuffer, std::initializer_list<Semaphore_Vulkan*> waitSemaphores);		
	}

#if defined (GRAPHICS_VULKAN)
	using namespace Vulkan;
#endif
}