#include "BlazeEngine/Graphics/OpenGL/OpenGLShader.h"
#include "BlazeEngine/DataStructures/ByteStream.h"
#include "BlazeEngine/Utilities/File.h"
#include "BlazeEngine/Core/Logger.h"

#include "GL/glew.h"

namespace Blaze
{
	namespace OpenGL
	{
		inline Shader::Shader(ShaderType type)
			: id(-1)
		{			
			GL_TESS_EVALUATION_SHADER;
			GL_TESS_CONTROL_SHADER;
			id = glCreateShader((uint)type);
		}		
		inline Shader::Shader(Shader&& s) noexcept
			: id(-1)
		{
			id = s.id;
			s.id = -1;
		}
		inline Shader::Shader(ShaderType type, const StringView& path, bool emitLogOnError)
			: Shader(type)
		{
			Load(path, emitLogOnError);
		}
		inline Shader::~Shader()
		{
			if (id != -1)
				glDeleteShader(id);
		}

		inline bool Shader::Load(const StringView& path, bool emitLogOnError)
		{
			File file;
			if (file.Open(path, FileOpenMode::Read) == BLAZE_ERROR)
			{
				return false;
			}

			Buffer buffer;
			buffer.Allocate(file.Size());
			size_t read = file.Read(buffer);

			ShaderSource(buffer);

			if (CompileShader() == BLAZE_ERROR)
			{
				if (emitLogOnError)
				{
					String log = GetCompilationLog();
					BLAZE_ERROR_LOG("Blaze Engine", std::move(log));
				}

				return BLAZE_ERROR;
			}

			return BLAZE_OK;
		}

		inline void Shader::ShaderSource(BufferView source)
		{
			const int lenght = source.Size();
			const char* ptr = (const char*)source.Ptr();
			glShaderSource(id, 1, &ptr, &lenght);
		}
		 
		inline void Shader::ShaderSource(StringView source)
		{
			const int lenght = source.Size();
			const char* ptr = source.Ptr();
			glShaderSource(id, 1, &ptr, &lenght);
		}

		inline int Shader::CompileShader()
		{			
			glCompileShader(id);
			int result;
			glGetShaderiv(id, GL_COMPILE_STATUS, &result);
			return (result == GL_TRUE ? BLAZE_OK : BLAZE_ERROR);
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
			if (id != -1)
				glDeleteShader(id);
			id = s.id;
			s.id = -1;

			return *this;
		}
	}
}