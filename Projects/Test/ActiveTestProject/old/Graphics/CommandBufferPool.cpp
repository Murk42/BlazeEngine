#include "CommandBufferPool.h"
#include "API Specific/Vulkan/Vulkan_CommandBufferPool.h"
#include "RenderContext.h"

namespace Experimental::Graphics
{
	using namespace Vulkan;

	CommandBufferPool::CommandBufferPool(RenderContext& renderContext, CommandQueueType type)
	{
		impl = new CommandBufferPoolImpl((RenderContextImpl*)renderContext.GetHandle(), type);
	}
	CommandBufferPool::~CommandBufferPool()
	{
		delete (CommandBufferPoolImpl*)impl;
	}	
}