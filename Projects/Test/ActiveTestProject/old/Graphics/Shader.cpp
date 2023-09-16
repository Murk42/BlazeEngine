#include "Shader.h"
#include "API Specific/Vulkan/Vulkan_Shader.h"
#include "RenderContext.h"

namespace Experimental::Graphics
{
	using namespace Vulkan;

	Shader::Shader(RenderContext& context, Path path, ShaderType type)
		: type(type)
	{
		impl = new ShaderImpl((RenderContextImpl*)context.GetHandle(), path);
	}
	Shader::~Shader()
	{
		delete (ShaderImpl*)impl;
	}
}