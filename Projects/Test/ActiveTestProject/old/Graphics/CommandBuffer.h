#pragma once
#include "CommandBufferPool.h"

namespace Experimental::Graphics
{
	class RenderPass;
	class RenderContext;
	class Framebuffer;
	
	class CommandBuffer
	{
	public:
		CommandBuffer(CommandBufferPool& pool);
		~CommandBuffer();

		void Record(RenderPass& renderPass, Framebuffer& renderTarget, Recti rect);
		void Reset();

		inline void* GetHandle() const { return impl; }
	private:
		void* impl;
	};
}