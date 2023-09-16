#pragma once
#include "CommandQueue.h"

namespace Experimental::Graphics
{
	class CommandBufferPool
	{
	public:
		CommandBufferPool(RenderContext& renderContext, CommandQueueType type);
		~CommandBufferPool();

		inline void* GetHandle() const { return impl; }
	private:
		void* impl;
	};
}