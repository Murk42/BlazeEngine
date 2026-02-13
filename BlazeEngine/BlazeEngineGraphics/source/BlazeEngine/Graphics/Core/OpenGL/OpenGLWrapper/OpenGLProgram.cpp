#include "pch.h"
#include "BlazeEngine/Core/Debug/Logger.h"
#include "BlazeEngine/Core/Debug/LoggerListener.h"
#include "BlazeEngine/Core/Common/Buffer.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLProgram.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLShader.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLConversions.h"

namespace Blaze::Graphics::OpenGL
{
	ShaderProgram::ShaderProgram()
		: id(0), state(ShaderProgramState::Invalid)
	{
		id = glCreateProgram();
	}
	ShaderProgram::ShaderProgram(uint id)
		: id(0), state(ShaderProgramState::Invalid)
	{
	}
	ShaderProgram::ShaderProgram(ShaderProgram&& p) noexcept
		: id(p.id), state(p.state)
	{
		p.id = 0;
		p.state = ShaderProgramState::Invalid;
	}
	ShaderProgram::ShaderProgram(ArrayView<Shader&> shaders)
		: ShaderProgram()
	{
		LinkShaders(shaders);
	}
	ShaderProgram::~ShaderProgram()
	{
		if (id != 0)
			glDeleteProgram(id);
	}

	void ShaderProgram::AttachShader(const Shader& shader)
	{
		glAttachShader(id, shader.GetHandle());
	}

	void ShaderProgram::DetachShader(const Shader& shader)
	{
		glDetachShader(id, shader.GetHandle());
	}

	void ShaderProgram::LinkProgram()
	{
		if (state == ShaderProgramState::SuccesfullyLinked)
			BLAZE_LOG_WARNING("Relinking a shader program. This might cause driver errors. Recreate the shader program before linking again");

		state = ShaderProgramState::UnsuccesfullyLinked;

		glLinkProgram(id);

		int linkStatus = 0;
		glGetProgramiv(id, GL_LINK_STATUS, &linkStatus);

		if (linkStatus == GL_FALSE)
		{
			int length = 0;
			glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);

			String message(length);

			glGetProgramInfoLog(id, length, &length, message.Ptr());

			BLAZE_LOG_WARNING("Failed to compile shader program with message:\n" + message);
			return;
		}

		state = ShaderProgramState::SuccesfullyLinked;
	}

	void ShaderProgram::LinkShaders(ArrayView<Shader&> shaders)
	{
		for (auto& s : shaders)
			AttachShader(s);

		LinkProgram();

		for (auto& s : shaders)
			DetachShader(s);
	}

	uint ShaderProgram::GetUniformCount() const
	{
		int count = 0;
		glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &count);

		return count;
	}

	int ShaderProgram::GetUniformLocation(const StringView& name)
	{
		return glGetUniformLocation(id, name.Ptr());
	}

	void ShaderProgram::GetUniformData(uint index, String& name, uint& uniformSize, UniformType& uniformType) const
	{
		int maxUniformNameLength = 0;
		glGetProgramiv(id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformNameLength);
		Buffer buffer(maxUniformNameLength);

		int length = 0;
		int size = 0;
		uint type = 0;

		glGetActiveUniform(id, index, maxUniformNameLength, &length, &size, &type, (char*)buffer.Ptr());

		name = StringView((char*)buffer.Ptr(), length);
		uniformSize = size;
		uniformType = MapOpenGLUniformType(type);
	}

	uint ShaderProgram::GetUniformBlockCount() const
	{
		int uniformBlockCount;
		glGetProgramInterfaceiv(id, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &uniformBlockCount);
		return uniformBlockCount;
	}

	void ShaderProgram::GetUniformBlockData(uint index, int& location, String& name, uint& uniformBlockSize, Array<int>& memberIndices) const
	{
		int maxUniformNameLength = 0;
		glGetProgramInterfaceiv(id, GL_UNIFORM_BLOCK, GL_MAX_NAME_LENGTH, &maxUniformNameLength);
		Buffer buffer(maxUniformNameLength);

		int length = 0;

		glGetActiveUniformBlockName(id, index, maxUniformNameLength, &length, (char*)buffer.Ptr());

		name = StringView((char*)buffer.Ptr(), length);

		constexpr const unsigned props1[]{ GL_NUM_ACTIVE_VARIABLES, GL_BUFFER_DATA_SIZE };
		constexpr const unsigned props2[]{ GL_ACTIVE_VARIABLES };

		struct {
			int uniformBlockVariableCount;
			int uniformBlockSize;
		} v1;

		glGetProgramResourceiv(id, GL_UNIFORM_BLOCK, index, 2, props1, 2, &length, (int*)&v1);

		memberIndices.Resize(v1.uniformBlockVariableCount);
		glGetProgramResourceiv(id, GL_UNIFORM_BLOCK, index, 1, props2, v1.uniformBlockVariableCount, &length, memberIndices.Ptr());

		uniformBlockSize = v1.uniformBlockSize;
	}

	void ShaderProgram::GetUniformBlockMemberData(int location, String& name, uint& size, UniformType& type, uint& offset)
	{
		constexpr const unsigned props1[]{ GL_NAME_LENGTH, GL_OFFSET, GL_TYPE, GL_ARRAY_SIZE };

		struct {
			int uniformBlockVariableNameLenght;
			int uniformBlockVariableOffset;
			int uniformBlockVariableType;
			int uniformBlockVariableSize;
		} v2;
		int lenght;

		glGetProgramResourceiv(id, GL_UNIFORM, location, 4, props1, 4, &lenght, (int*)&v2);

		char* ptr = new char[v2.uniformBlockVariableNameLenght];

		glGetActiveUniformName(id, location, v2.uniformBlockVariableNameLenght, &lenght, ptr);

		name = std::move(String(ptr, v2.uniformBlockVariableNameLenght));
		delete[] ptr;
		size = v2.uniformBlockVariableSize;
		type = MapOpenGLUniformType(v2.uniformBlockVariableType);
		offset = v2.uniformBlockVariableOffset;
	}

	void ShaderProgram::BindUniformBlock(int location, uint binding)
	{
		glUniformBlockBinding(id, location, binding);
	}

	void ShaderProgram::SetUniform(int location, const int& value)
	{
		glProgramUniform1i(id, location, value);
	}
	void ShaderProgram::SetUniform(int location, const uint& value)
	{
		glProgramUniform1ui(id, location, value);
	}
	void ShaderProgram::SetUniform(int location, const float& value)
	{
		glProgramUniform1f(id, location, value);
	}
	void ShaderProgram::SetUniform(int location, const double& value)
	{
		glProgramUniform1d(id, location, value);
	}
	void ShaderProgram::SetUniform(int location, const Vec2u& value)
	{
		glProgramUniform2ui(id, location, value.x, value.y);
	}
	void ShaderProgram::SetUniform(int location, const Vec2i& value)
	{
		glProgramUniform2i(id, location, value.x, value.y);
	}
	void ShaderProgram::SetUniform(int location, const Vec2f& value)
	{
		glProgramUniform2f(id, location, value.x, value.y);
	}
	void ShaderProgram::SetUniform(int location, const Vec2d& value)
	{
		glProgramUniform2d(id, location, value.x, value.y);
	}
	void ShaderProgram::SetUniform(int location, const Vec3u& value)
	{
		glProgramUniform3ui(id, location, value.x, value.y, value.z);
	}
	void ShaderProgram::SetUniform(int location, const Vec3i& value)
	{
		glProgramUniform3i(id, location, value.x, value.y, value.z);
	}
	void ShaderProgram::SetUniform(int location, const Vec3f& value)
	{
		glProgramUniform3f(id, location, value.x, value.y, value.z);
	}
	void ShaderProgram::SetUniform(int location, const Vec3d& value)
	{
		glProgramUniform3d(id, location, value.x, value.y, value.z);
	}
	void ShaderProgram::SetUniform(int location, const Vec4u& value)
	{
		glProgramUniform4ui(id, location, value.x, value.y, value.z, value.w);
	}
	void ShaderProgram::SetUniform(int location, const Vec4i& value)
	{
		glProgramUniform4i(id, location, value.x, value.y, value.z, value.w);
	}
	void ShaderProgram::SetUniform(int location, const Vec4f& value)
	{
		glProgramUniform4f(id, location, value.x, value.y, value.z, value.w);
	}
	void ShaderProgram::SetUniform(int location, const Vec4d& value)
	{
		glProgramUniform4d(id, location, value.x, value.y, value.z, value.w);
	}
	void ShaderProgram::SetUniform(int location, const Mat2f& value)
	{
		glProgramUniformMatrix2fv(id, location, 1, true, (float*)&value);
	}
	void ShaderProgram::SetUniform(int location, const Mat3f& value)
	{
		glProgramUniformMatrix3fv(id, location, 1, true, (float*)&value);
	}
	void ShaderProgram::SetUniform(int location, const Mat4f& value)
	{
		glProgramUniformMatrix4fv(id, location, 1, true, (float*)&value);
	}

	uint ShaderProgram::ReleaseHandleOwnership()
	{
		uint _id = id;
		id = 0;
		return _id;
	}

	void ShaderProgram::Release()
	{
		glDeleteProgram(id);
		id = 0;
	}

	ShaderProgram& ShaderProgram::operator=(ShaderProgram&& p) noexcept
	{
		if (id != 0)
			glDeleteProgram(id);

		id = p.id;
		p.id = 0;

		state = p.state;
		p.state = ShaderProgramState::Invalid;

		return *this;
	}
}