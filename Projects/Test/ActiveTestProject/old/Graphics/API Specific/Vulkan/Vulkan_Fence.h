#pragma once
#include "vulkan/vulkan.h"
#include "Vulkan_RenderContext.h"
#include "../../Fence.h"

namespace Experimental::Graphics::Vulkan
{
	struct FenceImpl
	{
		RenderContextImpl* renderContext;

		VkFence fence;

		FenceImpl(RenderContextImpl* renderContext, FenceSignalState state);
		~FenceImpl();

		void Wait();
		void Reset();
	};
}
