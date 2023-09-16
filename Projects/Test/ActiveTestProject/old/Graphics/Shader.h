#pragma once

namespace Experimental::Graphics
{
	class RenderContext;

	enum class ShaderType
	{
		Vertex, Fragment
	};

	class Shader
	{
	public:
		Shader(RenderContext& context, Path path, ShaderType type);
		~Shader();

		const ShaderType type;

		inline void* GetHandle() const { return impl; }
	private:
		void* impl;
	};
}