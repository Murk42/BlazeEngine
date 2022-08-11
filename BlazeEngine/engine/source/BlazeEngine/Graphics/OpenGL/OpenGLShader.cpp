#include "BlazeEngine/Graphics/OpenGL/OpenGLShader.h"
#include "BlazeEngine/File/File.h"
#include "BlazeEngine/Logging/Logger.h"
#include "BlazeEngine/Logging/LogListener.h"

#include "GL/glew.h"

#include <utility>

namespace Blaze
{
	namespace OpenGL
	{
		Shader::Shader(ShaderType type)
			: id(-1)
		{						
			id = glCreateShader((uint)type);
		}		
		Shader::Shader(Shader&& s) noexcept
			: id(-1)
		{
			id = s.id;
			s.id = -1;
		}
		Shader::Shader(ShaderType type, const Path& path)
			: Shader(type)
		{
			Load(path);
		}
		Shader::~Shader()
		{
			if (id != -1)
				glDeleteShader(id);
		}

		Result Shader::Load(const Path& path)
		{
			File file;
			if (Result result = file.Open(path, FileOpenMode::Read))
			{				
				return Result(result, Log(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE,
					"Blaze Engine", "Failed to open shader file with path \"" + path.GetString() + "\""), true);
			}

			Buffer buffer;
			buffer.Allocate(file.Size());
			size_t read = file.Read(buffer);

			ShaderSource(buffer);

			if (Result result = CompileShader())
			{				
				return Result(result, Log(LogType::Info, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE,
					"Blaze Engine", GetCompilationLog()));
			}

			return Result();
		}

		void Shader::ShaderSource(BufferView source)
		{
			const int lenght = source.Size();
			const char* ptr = (const char*)source.Ptr();
			glShaderSource(id, 1, &ptr, &lenght);
		}
		 
		void Shader::ShaderSource(StringView source)
		{
			const int lenght = source.Size();
			const char* ptr = source.Ptr();
			glShaderSource(id, 1, &ptr, &lenght);
		}

		Result Shader::CompileShader()
		{			
			LogListener listener;
			listener.StartListening();
			listener.SupressLogs(true);

			glCompileShader(id);
			int compileStatus;
			glGetShaderiv(id, GL_COMPILE_STATUS, &compileStatus);
			
			listener.StopListening();

			Result result;

			result.AddLogs(listener.GetLogs());						

			if (compileStatus == GL_FALSE)
				result.SetFailed(true);

			return result;
		}

		String Shader::GetCompilationLog()
		{
			int lenght;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght);
			String message(lenght);
			glGetShaderInfoLog(id, lenght, &lenght, message.Ptr());			
			return message;
		}

		Shader& Shader::operator=(Shader&& s) noexcept
		{
			if (id != -1)
				glDeleteShader(id);
			id = s.id;
			s.id = -1;

			return *this;
		}
	}
}