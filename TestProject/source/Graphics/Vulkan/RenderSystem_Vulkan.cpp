#include "pch.h"
#include "RenderSystem_Vulkan.h"

RenderSystem_Vulkan::RenderSystem_Vulkan() :
	line2DRenderer(gc, rp.GetRenderPass()), rp(gc, rw), nodeRenderTask(line2DRenderer)
{
}

void RenderSystem_Vulkan::SetNode(UI2::Node* rootNode)
{
	nodeRenderTask.SetNode(rootNode);
}

void RenderSystem_Vulkan::Frame()
{
	rp.Execute(nodeRenderTask);
}
