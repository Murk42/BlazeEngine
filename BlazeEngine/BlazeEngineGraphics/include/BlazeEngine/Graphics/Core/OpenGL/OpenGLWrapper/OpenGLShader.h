#pragma once
#include "BlazeEngine/Core/String/StringView.h"
#include "BlazeEngine/Core/File/Path.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLEnums.h"

namespace Blaze::Graphics::OpenGL
{
	class BLAZE_API Shader
	{
	public:
		Shader(const Shader&) = delete;
		Shader(Shader&&) noexcept;
		Shader(uint id);
		Shader(ShaderType type);
		Shader(ShaderType type, const Path& path);
		Shader(ShaderType type, StringView source);
		~Shader();

		bool Load(const Path& path);
		bool SetSource(StringView source);

		[[nodiscard]]
		uint ReleaseHandleOwnership();
		void Release();
		uint GetHandle() const { return id; }

		Shader& operator=(const Shader&) = delete;
		Shader& operator=(Shader&&) noexcept;
	private:
		uint id;
	};

	class VertexShader : public Shader
	{
	public:
		VertexShader() : Shader(ShaderType::VertexShader) { }
		VertexShader(const Path& path) : Shader(ShaderType::VertexShader, path) { }
		VertexShader(StringView source) : Shader(ShaderType::VertexShader, source) {}
	};

	class FragmentShader : public Shader
	{
	public:
		FragmentShader() : Shader(ShaderType::FragmentShader) { }
		FragmentShader(const Path& path) : Shader(ShaderType::FragmentShader, path) { }
		FragmentShader(StringView source) : Shader(ShaderType::FragmentShader, source) {}
	};

	class GeometryShader : public Shader
	{
	public:
		GeometryShader() : Shader(ShaderType::GeometryShader) { }
		GeometryShader(const Path& path) : Shader(ShaderType::GeometryShader, path) { }
		GeometryShader(StringView source) : Shader(ShaderType::GeometryShader, source) {}
	};

	class TessellationControlShader : public Shader
	{
	public:
		TessellationControlShader() : Shader(ShaderType::TessellationControlShader) { }
		TessellationControlShader(const Path& path) : Shader(ShaderType::TessellationControlShader, path) { }
		TessellationControlShader(StringView source) : Shader(ShaderType::TessellationControlShader, source) {}
	};

	class TessellationEvaluationShader : public Shader
	{
	public:
		TessellationEvaluationShader() : Shader(ShaderType::TessellationEvaluationShader) { }
		TessellationEvaluationShader(const Path& path) : Shader(ShaderType::TessellationEvaluationShader, path) { }
		TessellationEvaluationShader(StringView source) : Shader(ShaderType::TessellationEvaluationShader, source) {}
	};

	class ComputeShader : public Shader
	{
	public:
		ComputeShader() : Shader(ShaderType::ComputeShader) { }
		ComputeShader(const Path& path) : Shader(ShaderType::ComputeShader, path) { }
		ComputeShader(StringView source) : Shader(ShaderType::ComputeShader, source) {}
	};
}