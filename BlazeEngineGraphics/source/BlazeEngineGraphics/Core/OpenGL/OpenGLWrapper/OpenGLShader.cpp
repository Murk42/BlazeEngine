#include "pch.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLShader.h"

namespace Blaze::Graphics::OpenGLWrapper
{
	Shader::Shader(ShaderType type)
		: id(0), state(ShaderState::Invalid)
	{
		id = glCreateShader((uint)type);		
	}
	Shader::Shader(Shader&& s) noexcept
		: id(s.id), state(s.state)
	{
		s.id = 0;
		s.state = ShaderState::Invalid;
	}
	Shader::Shader(ShaderType type, const Path& path)
		: Shader(type)
	{
		Load(path);
	}
	Shader::~Shader()
	{
		if (id != 0)
			glDeleteShader(id);
	}

	void Shader::Load(const Path& path)
	{
		File file;

		file.Open(path, FileAccessPermission::Read);			
		
		String source;
		source.Resize(file.GetSize());
		size_t read = file.Read(source.Ptr(), source.Count());

		ShaderSource(source);			
		CompileShader();			
	}

	void Shader::ShaderSource(StringView source)
	{
		const int length = (int)source.Count();
		const char* ptr = source.Ptr();
		glShaderSource(id, 1, &ptr, &length);		
	}

	void Shader::CompileShader()
	{
		glCompileShader(id);		

		int compileStatus = 0;
		glGetShaderiv(id, GL_COMPILE_STATUS, &compileStatus);		

		if (compileStatus == GL_FALSE)
		{
			state = ShaderState::UnsuccesfullyCompiled;
			BLAZE_ENGINE_GRAPHICS_WARNING("Shader unsuccessfully compiled. The compilation log is: \n" + GetCompilationLog());
		}
		else
			state = ShaderState::SuccesfullyCompiled;
	}

	String Shader::GetCompilationLog()
	{
		int lenght = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght);

		String message(lenght);
		glGetShaderInfoLog(id, lenght, &lenght, message.Ptr());		

		return message;
	}

	Shader& Shader::operator=(Shader&& s) noexcept
	{
		if (id != 0)		
			glDeleteShader(id);

		id = s.id;
		s.id = 0;

		state = s.state;
		s.state = ShaderState::Invalid;

		return *this;
	}
}