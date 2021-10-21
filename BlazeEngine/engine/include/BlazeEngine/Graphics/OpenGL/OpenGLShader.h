#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/DataStructures/Buffer.h"

namespace Blaze
{
	namespace OpenGL
	{
		enum class ShaderType
		{
			FragmentShader = 35632,
			VertexShader = 35633,
			GeometryShader = 36313,
			TessellationControlShader = 36488,
			TessellationEvaluationShader = 36487,
		};

		class BLAZE_API Shader
		{
			uint id;
		public:
			inline Shader(ShaderType type);
			inline Shader(const Shader&) = delete;
			inline Shader(Shader&&) noexcept;
			inline Shader(ShaderType type, const StringView& path, bool emitLogOnError = true);
			inline ~Shader();

			inline bool Load(const StringView& path, bool emitLogOnError = true);

			inline void ShaderSource(BufferView source);
			inline void ShaderSource(StringView source);
			inline int CompileShader();
			inline String GetCompilationLog();			

			inline Shader& operator=(const Shader&) = delete;
			inline Shader& operator=(Shader&&) noexcept;

			friend class Program;
		};

		class VertexShader : public Shader
		{ 
		public:
			inline VertexShader() : Shader(ShaderType::VertexShader) { }
			inline VertexShader(const StringView& path, bool emitLogOnError = true) : Shader(ShaderType::VertexShader, path, emitLogOnError) { }
		};

		class FragmentShader : public Shader
		{
		public:
			inline FragmentShader() : Shader(ShaderType::FragmentShader) { }
			inline FragmentShader(const StringView& path, bool emitLogOnError = true) : Shader(ShaderType::FragmentShader, path, emitLogOnError) { }
		};

		class GeometryShader : public Shader
		{
		public:
			inline GeometryShader() : Shader(ShaderType::GeometryShader) { }
			inline GeometryShader(const StringView& path, bool emitLogOnError = true) : Shader(ShaderType::GeometryShader, path, emitLogOnError) { }
		};

		class TessellationControlShader : public Shader
		{
		public:
			inline TessellationControlShader() : Shader(ShaderType::TessellationControlShader) { }
			inline TessellationControlShader(const StringView& path, bool emitLogOnError = true) : Shader(ShaderType::TessellationControlShader, path, emitLogOnError) { }
		};

		class TessellationEvaluationShader : public Shader
		{
		public:
			inline TessellationEvaluationShader() : Shader(ShaderType::TessellationEvaluationShader) { }
			inline TessellationEvaluationShader(const StringView& path, bool emitLogOnError = true) : Shader(ShaderType::TessellationEvaluationShader, path, emitLogOnError) { }
		};
	}
}