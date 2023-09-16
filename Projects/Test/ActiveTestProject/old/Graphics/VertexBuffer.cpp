#include "VertexBuffer.h"
#include "API Specific/Vulkan/Vulkan_VertexBuffer.h"
#include "RenderContext.h"
#include "CommandBufferPool.h"
#include "CommandQueue.h"

namespace Experimental::Graphics
{
	using namespace Vulkan;


	VertexBuffer::VertexBuffer(RenderContext& renderContext, const void* data, uint size, CommandBufferPool& pool, CommandQueue& queue)
	{
		impl = new VertexBufferImpl((RenderContextImpl*)renderContext.GetHandle(), data, size, (CommandBufferPoolImpl*)pool.GetHandle(), (CommandQueueImpl*)queue.GetHandle());
	}
	VertexBuffer::~VertexBuffer()
	{
		delete (VertexBufferImpl*)impl;
	}
}