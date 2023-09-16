#include "RenderPass.h"
#include "API Specific/Vulkan/Vulkan_RenderPass.h"
#include "RenderContext.h"

namespace Experimental::Graphics
{
	using namespace Vulkan;

	RenderPass::RenderPass(RenderContext& renderContext, ArrayView<RenderPassAttachment> attachments, ArrayView<RenderSubpass> subpasses, ArrayView<RenderSubpassDependency> dependencies)
	{
		impl = new RenderPassImpl((RenderContextImpl*)renderContext.GetHandle(), attachments, subpasses, dependencies);
	}
	RenderPass::~RenderPass()
	{
		delete (RenderPassImpl*)impl;
	}
}