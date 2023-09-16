#include "Vulkan_Fence.h"
#include "Vulkan_Instance.h"
#include <iostream>

namespace Experimental::Graphics::Vulkan
{
	FenceImpl::FenceImpl(RenderContextImpl* renderContext, FenceSignalState signalState)
		: renderContext(renderContext)
	{
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

		switch (signalState)
		{
		case Experimental::Graphics::FenceSignalState::Signaled: fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; break;
		case Experimental::Graphics::FenceSignalState::NotSignaled:	fenceInfo.flags = 0; break;
		}

		if (vkCreateFence(renderContext->device, &fenceInfo, nullptr, &fence) != VK_SUCCESS)
		{
			std::cout << "Failed to create fence\n";
			std::exit(1);
		}
	}
	FenceImpl::~FenceImpl()
	{
		vkDestroyFence(renderContext->device, fence, nullptr);
	}

	void FenceImpl::Wait()
	{
		vkWaitForFences(renderContext->device, 1, &fence, VK_TRUE, UINT64_MAX);
	}
	void FenceImpl::Reset()
	{
		vkResetFences(renderContext->device, 1, &fence);
	}
}