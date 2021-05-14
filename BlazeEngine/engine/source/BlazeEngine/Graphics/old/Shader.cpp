#include "BlazeEngine/Graphics/Shader.h"

#include "BlazeEngine/Core/Logger.h"
#include "BlazeEngine/Utilities/File.h"
#include "BlazeEngine/DataStructures/ByteStream.h"

#include "GL/glew.h"

namespace Blaze
{
	Shader::Shader(ShaderType type)
		: type(type)
	{		
		id = glCreateShader((uint)type);
	}
	Shader::Shader(ShaderType type, const StringView& path)
		: type(type)
	{
		id = glCreateShader((uint)type);
		LoadShader(path);
	}
	Shader::Shader(Shader&& s) noexcept
		: id(std::exchange(s.id, 0)), type(s.type), path(std::move(s.path))
	{
	}
	Shader::~Shader()
	{
		if (id != 0)
		glDeleteShader(id);
	}
	bool Shader::LoadShader(const StringView& path)
	{
		ByteStream bs;
		if (!File::Read(path, &bs))
		{
			Logger::AddLog(LogType::Warning, __FUNCTION__, "Failed to load shader");
			return false;
		}

		if (SetSource((char*)bs.GetData().data(), bs.GetSize()))
		{
			this->path = path;
			return true;
		}
		return false;
	}
	bool Shader::SetSource(char* data, size_t size)
	{				
		int lenght = int(size);
		glShaderSource(id, 1, &data, (int*)&lenght);

		glCompileShader(id);

		int result;
		glGetShaderiv(id, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE)
		{
			int lenght;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght);
			String message(lenght);			
			glGetShaderInfoLog(id, lenght, &lenght, message.Ptr());
			Logger::AddLog(LogType::Error, "Shader::LoadShader", message.Ptr());
			return false;		
		}		
		return true;
	}
	void Shader::operator=(Shader&& s) noexcept
	{
		id = std::exchange(s.id, 0);
		type = s.type;
		path = std::move(s.path);
	}
}