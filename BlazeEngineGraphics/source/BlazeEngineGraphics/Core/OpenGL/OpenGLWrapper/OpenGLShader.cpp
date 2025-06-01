#include "pch.h"
#include "BlazeEngineCore/DataStructures/String.h"
#include "BlazeEngineCore/Debug/Logger.h"
#include "BlazeEngineCore/File/File.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLShader.h"
#include <GL/glew.h>

namespace Blaze::Graphics::OpenGLWrapper
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
			BLAZE_ENGINE_GRAPHICS_ERROR("Shader unsuccessfully compiled. The compilation log is: {}", GetCompilationLog(id));
			return false;
		}
		else
			return true;
	}

	Shader::Shader(ShaderType type)
		: id(0)
	{
		id = glCreateShader((uint)type);		
	}
	Shader::Shader(Shader&& s) noexcept
		: id(s.id)
	{
		s.id = 0;
	}
	Shader::Shader(ShaderType type, const Path& path)
		: Shader(type)
	{
		Load(path);
	}
	Shader::Shader(ShaderType type, StringView source)
		: Shader(type)
	{
		Load(source);
	}
	Shader::~Shader()
	{
		if (id != 0)
			glDeleteShader(id);
	}

	bool Shader::Load(const Path& path)
	{
		File file;

		file.Open(path, FileAccessPermission::Read);			
		
		String source;
		source.Resize(file.GetSize());
		if (file.Read(source.Ptr(), source.Count()) != source.Count())
		{
			BLAZE_ENGINE_GRAPHICS_ERROR("Field to read file");
			return false;
		}

		return Load(source);
	}
	bool Shader::Load(StringView source)
	{
		const char* ptr = source.Ptr();
		int length = static_cast<int>(source.Count());

		glShaderSource(id, 1, &ptr, &length);

		return CompileShader(id);
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