#include "CommandBuffer.h"
#include "API Specific/Vulkan/Vulkan_CommandBuffer.h"
#include "API Specific/Vulkan/Vulkan_CommandBufferPool.h"
#include "RenderContext.h"
#include "Framebuffer.h"

namespace Experimental::Graphics
{
	struct RenderPassImpl;
	struct FramebufferImpl;
}
namespace Experimental::Graphics
{	
	using namespace Vulkan;


	CommandBuffer::CommandBuffer(CommandBufferPool& pool)
	{
		impl = new CommandBufferImpl((CommandBufferPoolImpl*)pool.GetHandle());
	}
	CommandBuffer::~CommandBuffer()
	{
		delete (CommandBufferImpl*)impl;
	}
	void CommandBuffer::Record(RenderPass& renderPass, Framebuffer& renderTarget, Recti rect)
	{
		((CommandBufferImpl*)impl)->Record((RenderPassImpl*)renderPass.GetHandle(), (FramebufferImpl*)renderTarget.GetHandle(), rect);
	}
	void CommandBuffer::Reset()
	{
		((CommandBufferImpl*)impl)->Reset();		
	}
}