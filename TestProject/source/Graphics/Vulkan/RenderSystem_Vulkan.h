#pragma once
#define GRAPHICS_VULKAN
#include "BlazeEngineGraphics/BlazeEngineGraphics.h"

#include "Pipelines/MainRenderPipeline_Vulkan.h"
#include "UI/NodeRenderTask_Vulkan.h"

class RenderSystem_Vulkan
{
	Graphics::Vulkan::GraphicsContext_Vulkan gc;
	Graphics::Vulkan::RenderWindow_Vulkan rw{ gc, "Marko", Vec2i(INT_MAX), Vec2u(640, 480) };

	MainRenderPipeline_Vulkan rp;

	Graphics::Vulkan::Line2DRenderer_Vulkan line2DRenderer;
	UI2::NodeRenderTask_Vulkan nodeRenderTask;
public:
	RenderSystem_Vulkan();

	WindowBase& GetWindow() { return rw.GetWindow(); }

	void SetNode(UI2::Node* rootNode);

	void Frame();
};