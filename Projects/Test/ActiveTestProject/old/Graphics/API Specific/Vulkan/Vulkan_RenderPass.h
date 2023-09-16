#pragma once
#include "vulkan/vulkan.h"
#include "Vulkan_RenderContext.h"
#include "../../RenderPass.h"

namespace Experimental::Graphics::Vulkan
{
	struct PipelineImpl;
	struct IndexBufferImpl;

	struct RenderPassImpl
	{
		RenderContextImpl* renderContext;
		VkRenderPass renderPass;
		Array<RenderSubpass> subpasses;

		RenderPassImpl(RenderContextImpl* renderContext, ArrayView<RenderPassAttachment> attachments, ArrayView<RenderSubpass> subpasses, ArrayView<RenderSubpassDependency> dependencies);
		~RenderPassImpl();
	};
}