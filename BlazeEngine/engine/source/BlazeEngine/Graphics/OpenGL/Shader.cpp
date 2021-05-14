#include "BlazeEngine/Graphics/OpenGL/Shader.h"
#include "BlazeEngine/DataStructures/ByteStream.h"
#include "BlazeEngine/Utilities/File.h"
#include "BlazeEngine/Core/Logger.h"

#include "GL/glew.h"

namespace Blaze
{
	namespace OpenGL
	{
		inline Shader::Shader(ShaderType type)
			: id(0)
		{
			id = glCreateShader((uint)type);
		}		
		inline Shader::Shader(Shader&& s) noexcept
			: id(0)
		{
			id = s.id;
			s.id = 0;
		}
		inline Shader::Shader(ShaderType type, const StringView& path, bool emitLogOnError)
			: Shader(type)
		{
			Load(path, emitLogOnError);
		}
		inline Shader::~Shader()
		{
			if (id != 0)
				glDeleteShader(id);
		}

		inline bool Shader::Load(const StringView& path, bool emitLogOnError)
		{
			ByteStream bs;
			if (File::Read(path, &bs, emitLogOnError) == BLAZE_ERROR)
				return false;

			ShaderSource((char*)bs.GetData().data(), bs.GetSize());

			if (CompileShader() == BLAZE_ERROR)
			{
				if (emitLogOnError)
				{
					String log = GetCompilationLog();
					Logger::AddLog(LogType::Error, "Shader::Load", std::move(log));
				}

				return BLAZE_ERROR;
			}

			return BLAZE_OK;
		}

		inline void Shader::ShaderSource(const char* ptr, uint lenght)
		{
			const int lenghtI = lenght;
			glShaderSource(id, 1, &ptr, &lenghtI);
		}

		inline int Shader::CompileShader()
		{
			glCompileShader(id);
			int result;
			glGetShaderiv(id, GL_COMPILE_STATUS, &result);
			return result;
		}

		inline String Shader::GetCompilationLog()
		{
			int lenght;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght);
			String message(lenght);
			glGetShaderInfoLog(id, lenght, &lenght, message.Ptr());			
			return message;
		}

		inline Shader& Shader::operator=(Shader&& s) noexcept
		{
			id = s.id;
			s.id = 0;

			return *this;
		}
	}
}