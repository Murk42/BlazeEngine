#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "BlazeEngine/DataStructures/String.h"

namespace Blaze
{
	namespace OpenGL
	{
		enum class ShaderType
		{
			FragmentShader = 35632,
			VertexShader = 35633,
			GeometryShader = 36313,
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

			inline void ShaderSource(const char* ptr, uint lenght);
			inline int CompileShader();
			inline String GetCompilationLog();			

			inline Shader& operator=(const Shader&) = delete;
			inline Shader& operator=(Shader&&) noexcept;

			friend class Program;
		};
	}
}