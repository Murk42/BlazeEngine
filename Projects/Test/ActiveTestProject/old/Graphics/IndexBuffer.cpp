#include "IndexBuffer.h"
#include "API Specific/Vulkan/Vulkan_IndexBuffer.h"
#include "RenderContext.h"
#include "CommandBufferPool.h"
#include "CommandQueue.h"

namespace Experimental::Graphics
{
	using namespace Vulkan;
	

	IndexBuffer::IndexBuffer(RenderContext& renderContext, const void* data, uint size, CommandBufferPool& pool, CommandQueue& queue)
	{
		impl = new IndexBufferImpl((RenderContextImpl*)renderContext.GetHandle(), data, size, (CommandBufferPoolImpl*)pool.GetHandle(), (CommandQueueImpl*)queue.GetHandle());
	}
	IndexBuffer::~IndexBuffer()
	{
		delete (IndexBufferImpl*)impl;
	}
}