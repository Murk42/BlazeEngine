#include "Fence.h"
#include "API Specific/Vulkan/Vulkan_Fence.h"
#include "RenderContext.h"

namespace Experimental::Graphics
{
	using namespace Vulkan;

	Fence::Fence(RenderContext& renderContext, FenceSignalState signalState)
	{
		impl = new FenceImpl((RenderContextImpl*)renderContext.GetHandle(), signalState);
	}

	Fence::~Fence()
	{
		delete (FenceImpl*)impl;
	}
	void Fence::Wait()
	{
		((FenceImpl*)impl)->Wait();
	}
	void Fence::Reset()
	{
		((FenceImpl*)impl)->Reset();
	}
}