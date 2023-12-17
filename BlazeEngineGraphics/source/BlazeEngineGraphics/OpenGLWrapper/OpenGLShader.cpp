#include "pch.h"
#include "BlazeEngineGraphics\OpenGLWrapper/OpenGLCommon.h"
#include "BlazeEngineGraphics\OpenGLWrapper/OpenGLShader.h"

namespace Blaze::Graphics::OpenGLWrapper
{
	Shader::Shader(ShaderType type)
		: id(-1), state(ShaderState::Invalid)
	{
		id = glCreateShader((uint)type);
		FlushOpenGLResult();
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
		Load(path);
	}
	Shader::~Shader()
	{
		if (id != -1)
		{
			glDeleteShader(id);
			FlushOpenGLResult();
		}
	}

	Result Shader::Load(const Path& path)
	{
		File file;

		CHECK_RESULT(file.Open(path, FileAccessPermission::Read));
		
		String source;
		source.Resize(file.GetSize());
		size_t read = file.Read(source.Ptr(), source.Count());

		CHECK_RESULT(ShaderSource(source));

		CHECK_RESULT(CompileShader());

		return Result();
	}

	Result Shader::ShaderSource(StringView source)
	{
		const int length = (int)source.Count();
		const char* ptr = source.Ptr();
		glShaderSource(id, 1, &ptr, &length);
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
		FlushOpenGLResult();

		String message(lenght);
		glGetShaderInfoLog(id, lenght, &lenght, message.Ptr());
		FlushOpenGLResult();

		return message;
	}

	Shader& Shader::operator=(Shader&& s) noexcept
	{
		if (id != -1)
		{
			glDeleteShader(id);

			if (FlushOpenGLResult())
				return *this;
		}

		id = s.id;
		s.id = -1;

		state = s.state;
		s.state = ShaderState::Invalid;

		return *this;
	}
}