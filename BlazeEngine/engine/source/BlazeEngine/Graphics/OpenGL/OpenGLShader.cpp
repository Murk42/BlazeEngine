#include "BlazeEngine/Graphics/OpenGL/OpenGLShader.h"
#include "BlazeEngine/File/File.h"


#include "GL/glew.h"

#include <utility>

namespace Blaze
{
	namespace OpenGL
	{
		Shader::Shader(ShaderType type)
			: id(-1), state(ShaderState::Invalid)
		{				
			id = glCreateShader((uint)type);
			FLUSH_OPENGL_RESULT();
		}		
		Shader::Shader(Shader&& s) noexcept
			: id(s.id), state(s.state)
		{			
			s.id = -1;
			s.state = ShaderState::Invalid;
		}
		Shader::Shader(ShaderType type, const Path& path)
			: Shader(type)
		{
			ADD_STACK_FRAME(Load(path));
		}
		Shader::~Shader()
		{
			if (id != -1)
			{
				glDeleteShader(id);
				FLUSH_OPENGL_RESULT();
			}
		}

		Result Shader::Load(const Path& path)
		{
			File file;

			CHECK_RESULT(file.Open(path, FileOpenMode::Read));			

			Buffer buffer;
			buffer.Allocate(file.Size());
			size_t read = file.Read(buffer);

			CHECK_RESULT(ShaderSource(StringView((const char*)buffer.Ptr(), buffer.Size())));

			CHECK_RESULT(CompileShader());			

			return Result();
		}
				 
		Result Shader::ShaderSource(StringView source)
		{
			const int lenght = source.Size();
			const char* ptr = source.Ptr();
			glShaderSource(id, 1, &ptr, &lenght);
			CHECK_OPENGL_RESULT();

			return Result();
		}

		Result Shader::CompileShader()
		{						
			glCompileShader(id);
			CHECK_OPENGL_RESULT();
			
			int compileStatus;
			glGetShaderiv(id, GL_COMPILE_STATUS, &compileStatus);			
			CHECK_OPENGL_RESULT();						
			
			if (compileStatus == GL_FALSE)
			{
				state = ShaderState::UnsuccesfullyCompiled;				
			}
			else
				state = ShaderState::SuccesfullyCompiled;

			return Result();
		}

		String Shader::GetCompilationLog()
		{
			int lenght;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght);
			FLUSH_OPENGL_RESULT();
			
			String message(lenght);
			glGetShaderInfoLog(id, lenght, &lenght, message.Ptr());			
			FLUSH_OPENGL_RESULT();
			
			return message;
		}

		Shader& Shader::operator=(Shader&& s) noexcept
		{
			if (id != -1)
			{
				glDeleteShader(id);

				if (FLUSH_OPENGL_RESULT())
					return *this;
			}

			id = s.id;
			s.id = -1;

			state = s.state;
			s.state = ShaderState::Invalid;

			return *this;
		}
	}
}