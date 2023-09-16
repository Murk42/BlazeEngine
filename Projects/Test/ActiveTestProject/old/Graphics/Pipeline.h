#pragma once
#include "VertexLayout.h"

namespace Experimental::Graphics
{	
	class RenderContext;
	class Shader;
	class RenderPass;

	class Pipeline
	{
	public:		
		Pipeline(RenderPass& renderPass, uint subpassIndex, ArrayView<Shader*> shaders, VertexLayout& vertexLayout, ArrayView<VertexBinding> bindings);
		~Pipeline();
	private:
		void* impl;
	};
}