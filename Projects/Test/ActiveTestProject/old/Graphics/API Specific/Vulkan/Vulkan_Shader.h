#pragma once
#include "vulkan/vulkan.h"
#include "Vulkan_RenderContext.h"

namespace Experimental::Graphics::Vulkan
{
	struct ShaderImpl
	{
		RenderContextImpl* renderContext;

		VkShaderModule shader;

		ShaderImpl(RenderContextImpl* renderContext, const Path& path);
		~ShaderImpl();
	};
}