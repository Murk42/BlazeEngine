#pragma once
#include "vulkan/vulkan.h"
#include "Vulkan_RenderContext.h"
#include "../../VertexLayout.h"

namespace Experimental::Graphics
{
	class Shader;
	struct VertexLayout;
}

namespace Experimental::Graphics::Vulkan
{
	struct RenderPassImpl;

	struct PipelineImpl
	{
		RenderContextImpl* renderContext;
		VkPipeline pipeline;
		VkPipelineLayout layout;

		PipelineImpl(RenderPassImpl* renderPass, uint subpassIndex, ArrayView<Shader*> shaders, VertexLayout& vertexLayout, ArrayView<VertexBinding> bindings);
		~PipelineImpl();
	};
}