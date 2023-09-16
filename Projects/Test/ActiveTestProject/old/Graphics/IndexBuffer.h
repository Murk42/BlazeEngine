#pragma once

namespace Experimental::Graphics
{
	class RenderContext;
	class CommandBufferPool;
	class CommandQueue;

	class IndexBuffer
	{
	public:
		IndexBuffer(RenderContext& renderContext, const void* data, uint size, CommandBufferPool& pool, CommandQueue& queue);
		~IndexBuffer();
	private:
		void* impl;
	};
}