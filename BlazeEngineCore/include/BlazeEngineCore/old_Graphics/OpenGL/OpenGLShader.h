#pragma once
#include "BlazeEngineCore/DataStructures/Buffer.h"
#include "BlazeEngineCore/File/Path.h"

namespace Blaze::OpenGL
{
	enum class ShaderType
	{
		FragmentShader = 35632,
		VertexShader = 35633,
		GeometryShader = 36313,
		TessellationControlShader = 36488,
		TessellationEvaluationShader = 36487,
		ComputeShader = 37305,
	};

	enum class ShaderState
	{
		Invalid,
		UnsuccesfullyCompiled,
		SuccesfullyCompiled,
	};

	class BLAZE_CORE_API Shader
	{
		uint id;
		ShaderState state;
	public:
		Shader(ShaderType type);
		Shader(const Shader&) = delete;
		Shader(Shader&&) noexcept;
		Shader(ShaderType type, const Path& path);
		~Shader();

		Result Load(const Path& path);

		Result ShaderSource(StringView source);
		Result CompileShader();
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
		VertexShader(const StringView& path) : Shader(ShaderType::VertexShader, path) { }
	};

	class FragmentShader : public Shader
	{
	public:
		FragmentShader() : Shader(ShaderType::FragmentShader) { }
		FragmentShader(const StringView& path) : Shader(ShaderType::FragmentShader, path) { }
	};

	class GeometryShader : public Shader
	{
	public:
		GeometryShader() : Shader(ShaderType::GeometryShader) { }
		GeometryShader(const StringView& path) : Shader(ShaderType::GeometryShader, path) { }
	};

	class TessellationControlShader : public Shader
	{
	public:
		TessellationControlShader() : Shader(ShaderType::TessellationControlShader) { }
		TessellationControlShader(const StringView& path) : Shader(ShaderType::TessellationControlShader, path) { }
	};

	class TessellationEvaluationShader : public Shader
	{
	public:
		TessellationEvaluationShader() : Shader(ShaderType::TessellationEvaluationShader) { }
		TessellationEvaluationShader(const StringView& path) : Shader(ShaderType::TessellationEvaluationShader, path) { }
	};

	class ComputeShader : public Shader
	{
	public:
		ComputeShader() : Shader(ShaderType::ComputeShader) { }
		ComputeShader(const StringView& path) : Shader(ShaderType::ComputeShader, path) { }
	};
}