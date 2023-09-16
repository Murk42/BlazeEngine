#include "Vulkan_Fence.h"
#include "Vulkan_Instance.h"
#include <iostream>

namespace Graphics2
{
	Vulkan_Fence::Vulkan_Fence(Vulkan_RenderContext* renderContext)
		: renderContext(renderContext)
	{
	}
	Vulkan_Fence::~Vulkan_Fence()
	{
		Destroy();
	}
	void Vulkan_Fence::Create(FenceSignalState signalState)
	{
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

		switch (signalState)
		{
		case Graphics2::FenceSignalState::Signaled: fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; break;
		case Graphics2::FenceSignalState::NotSignaled:	fenceInfo.flags = 0; break;
		}

		if (vkCreateFence(renderContext->device, &fenceInfo, nullptr, &fence) != VK_SUCCESS)
		{
			std::cout << "Failed to create fence\n";
			std::exit(1);
		}
	}
	void Vulkan_Fence::Destroy()
	{
		vkDestroyFence(renderContext->device, fence, nullptr);
	}
	void Vulkan_Fence::Wait()
	{
		vkWaitForFences(renderContext->device, 1, &fence, VK_TRUE, UINT64_MAX);
	}
	void Vulkan_Fence::Reset()
	{
		vkResetFences(renderContext->device, 1, &fence);
	}
}