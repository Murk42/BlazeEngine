#pragma once
#include "BlazeEngineGraphics\Dynamic/Core/GraphicsContext.h"
#include "vulkan/vulkan.h"

#define CHECK_VULKAN(x) if (x != VK_SUCCESS) Debug::Logger::LogError("BlazeEngine", "Vulkan function failed");

namespace Blaze::Graphics::Vulkan
{
	struct GraphicsContextProperties_Vulkan
	{

	};

	class BLAZE_GRAPHICS_API GraphicsContext_Vulkan : public Blaze::Graphics::Dynamic::GraphicsContextBase
	{
	public:
		/*Parity*/GraphicsContext_Vulkan();
		/*Parity*/~GraphicsContext_Vulkan() override;

		/*Parity*/inline String GetImplementationName() const override final { return "OpenGL"; }

		GraphicsContext_Vulkan(const GraphicsContextProperties_Vulkan& properties);

		void CreateWindowAndSurface(WindowSDL& window, VkSurfaceKHR& surface, StringView title, Vec2i pos, Vec2u size);
		void DestroyWindowAndSurface(WindowSDL& window, VkSurfaceKHR surface);

		void RunSingleTimeCommand(const std::function<void(VkCommandBuffer)>&);
		void TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage, VkAccessFlags srcAccess, VkAccessFlags dstAccess);

		inline VkDevice GetDevice() const { return device; }
		inline VkPhysicalDevice GetPhysicalDevice() const { return physicalDevice; }
		inline uint32 GetGraphicsQueueFamily() const { return graphicsQueueFamily; }
		inline uint32 GetPresentQueueFamily() const { return presentQueueFamily; }
		inline const Set<String>& GetEnabledExtensions() const { return enabledExtensions; }
	private:
		VkDevice device;
		VkPhysicalDevice physicalDevice;
		Set<String> enabledExtensions;
		uint32 graphicsQueueFamily;
		uint32 presentQueueFamily;

		WindowSDL initWindow;
		VkSurfaceKHR initSurface;				

		VkQueue graphicsQueue;
		VkCommandPool commandPool;
	};
}