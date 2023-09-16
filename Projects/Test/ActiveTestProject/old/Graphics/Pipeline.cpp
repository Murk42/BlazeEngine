#include "Pipeline.h"
#include "API Specific/Vulkan/Vulkan_Pipeline.h"
#include "RenderPass.h"
#include "RenderContext.h"

namespace Experimental::Graphics
{
	using namespace Vulkan;

	Pipeline::Pipeline(RenderPass& renderPass, uint subpassIndex, ArrayView<Shader*> shaders, VertexLayout& vertexLayout, ArrayView<VertexBinding> bindings)
	{
		impl = new PipelineImpl((RenderPassImpl*)renderPass.GetHandle(), subpassIndex, shaders, vertexLayout, bindings);
	}
	Pipeline::~Pipeline()
	{
		delete (PipelineImpl*)impl;
	}
}