#pragma once
#define GRAPHICS_VULKAN
#include "BlazeEngineGraphics/BlazeEngineGraphics.h"
#include "UI/NodeRenderTask_Vulkan.h"
using namespace Blaze::Graphics;

class MainRenderPipeline_Vulkan
{
public:
	MainRenderPipeline_Vulkan(
		Vulkan::GraphicsContext_Vulkan& graphicsContext,
		Vulkan::RenderWindow_Vulkan& renderWindow		
	);
	~MainRenderPipeline_Vulkan();

	void Execute(UI2::NodeRenderTask_Vulkan& nodeRenderTask);

	inline VkRenderPass GetRenderPass() const { return renderPass; }
private:
	Vulkan::GraphicsContext_Vulkan& graphicsContext;
	Vulkan::RenderWindow_Vulkan& renderWindow;	

	static constexpr uint framesInFlight = 1;

	VkRenderPass renderPass;
	VkQueue queue;

	Vulkan::Semaphore_Vulkan rwfbSemaphores[framesInFlight];
	Vulkan::Semaphore_Vulkan presentSemaphores[framesInFlight];
	uint currentFrame;
	VkCommandPool commandPool;
	VkCommandBuffer commandBuffers[framesInFlight];
	VkFence fences[framesInFlight];
};