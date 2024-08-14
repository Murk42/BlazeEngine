#pragma once
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLEnums.h"

namespace Blaze::Graphics::OpenGLWrapper
{	
	class BLAZE_GRAPHICS_API Shader
	{
		uint id;
		ShaderState state;
	public:
		Shader(ShaderType type);
		Shader(const Shader&) = delete;
		Shader(Shader&&) noexcept;
		Shader(ShaderType type, const Path& path);
		~Shader();

		void Load(const Path& path);

		void ShaderSource(StringView source);
		void CompileShader();
		String GetCompilationLog();

		inline ShaderState GetState() const { return state; }

		Shader& operator=(const Shader&) = delete;
		Shader& operator=(Shader&&) noexcept;

		friend class ShaderProgram;
	};

	class VertexShader : public Shader
	{
	public:
		VertexShader() : Shader(ShaderType::VertexShader) { }
		VertexShader(const Path& path) : Shader(ShaderType::VertexShader, path) { }
	};

	class FragmentShader : public Shader
	{
	public:
		FragmentShader() : Shader(ShaderType::FragmentShader) { }
		FragmentShader(const Path& path) : Shader(ShaderType::FragmentShader, path) { }
	};

	class GeometryShader : public Shader
	{
	public:
		GeometryShader() : Shader(ShaderType::GeometryShader) { }
		GeometryShader(const Path& path) : Shader(ShaderType::GeometryShader, path) { }
	};

	class TessellationControlShader : public Shader
	{
	public:
		TessellationControlShader() : Shader(ShaderType::TessellationControlShader) { }
		TessellationControlShader(const Path& path) : Shader(ShaderType::TessellationControlShader, path) { }
	};

	class TessellationEvaluationShader : public Shader
	{
	public:
		TessellationEvaluationShader() : Shader(ShaderType::TessellationEvaluationShader) { }
		TessellationEvaluationShader(const Path& path) : Shader(ShaderType::TessellationEvaluationShader, path) { }
	};

	class ComputeShader : public Shader
	{
	public:
		ComputeShader() : Shader(ShaderType::ComputeShader) { }
		ComputeShader(const Path& path) : Shader(ShaderType::ComputeShader, path) { }
	};
}