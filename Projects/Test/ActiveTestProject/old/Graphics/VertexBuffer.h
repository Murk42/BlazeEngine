#pragma once

namespace Experimental::Graphics
{
	class RenderContext;
	class CommandBufferPool;
	class CommandQueue;

	class VertexBuffer
	{
	public:
		VertexBuffer(RenderContext& renderContext, const void* data, uint size, CommandBufferPool& pool, CommandQueue& queue);
		~VertexBuffer();
	private:
		void* impl;
	};
}