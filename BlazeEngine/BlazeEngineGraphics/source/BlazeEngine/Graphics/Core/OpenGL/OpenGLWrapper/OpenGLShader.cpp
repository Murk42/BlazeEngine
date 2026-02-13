#include "pch.h"
#include "BlazeEngine/Core/String/String.h"
#include "BlazeEngine/Core/Debug/Logger.h"
#include "BlazeEngine/Core/File/File.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLShader.h"
#include <GL/glew.h>

namespace Blaze::Graphics::OpenGL
{
	static String GetCompilationLog(uint id)
	{
		int lenght = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght);

		String message(lenght);
		glGetShaderInfoLog(id, lenght, &lenght, message.Ptr());

		return message;
	}

	static bool CompileShader(uint id)
	{
		glCompileShader(id);

		int compileStatus = 0;
		glGetShaderiv(id, GL_COMPILE_STATUS, &compileStatus);

		if (compileStatus == GL_FALSE)
		{
			BLAZE_LOG_ERROR("Shader unsuccessfully compiled. The compilation log is: {}", GetCompilationLog(id));
			return false;
		}
		else
			return true;
	}

	Shader::Shader(Shader&& s) noexcept
		: id(s.id)
	{
		s.id = 0;
	}
	Shader::Shader(uint id)
		: id(id)
	{
	}
	Shader::Shader(ShaderType type)
		: id(0)
	{
		id = glCreateShader((uint)type);
	}
	Shader::Shader(ShaderType type, const Path& path)
		: Shader(type)
	{
		Load(path);
	}
	Shader::Shader(ShaderType type, StringView source)
		: Shader(type)
	{
		SetSource(source);
	}
	Shader::~Shader()
	{
		Release();
	}
	bool Shader::Load(const Path& path)
	{
		File file;

		file.Open(path, FileAccessPermission::Read);

		String source;
		source.Resize(file.GetSize());
		if (file.Read(source.Ptr(), source.Count()) != source.Count())
		{
			BLAZE_LOG_ERROR("Field to read file");
			return false;
		}

		return SetSource(source);
	}
	bool Shader::SetSource(StringView source)
	{
		const char* ptr = source.Ptr();
		int length = static_cast<int>(source.Count());

		glShaderSource(id, 1, &ptr, &length);

		return CompileShader(id);
	}
	uint Shader::ReleaseHandleOwnership()
	{
		uint _id = id;
		id = 0;
		return _id;
	}
	void Shader::Release()
	{
		if (id != 0)
			glDeleteShader(id);
	}

	Shader& Shader::operator=(Shader&& s) noexcept
	{
		if (id != 0)
			glDeleteShader(id);

		id = s.id;
		s.id = 0;

		return *this;
	}
}