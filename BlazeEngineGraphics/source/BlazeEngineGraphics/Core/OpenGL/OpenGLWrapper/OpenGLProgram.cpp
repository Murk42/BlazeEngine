#include "pch.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLContext.h"

namespace Blaze::Graphics::OpenGLWrapper
{
	ShaderProgram::ShaderProgram()
		: id(-1), state(ShaderProgramState::Invalid), maxUniformBlockNameLenght(0), maxUniformNameLenght(0)
	{
		id = glCreateProgram();
		
	}
	ShaderProgram::ShaderProgram(ShaderProgram&& p) noexcept
		: id(p.id), state(p.state), maxUniformBlockNameLenght(p.maxUniformBlockNameLenght), maxUniformNameLenght(p.maxUniformNameLenght)
	{
		p.id = -1;
		p.state = ShaderProgramState::Invalid;
		p.maxUniformBlockNameLenght = 0;
		p.maxUniformNameLenght = 0;
	}
	ShaderProgram::~ShaderProgram()
	{
		if (id != -1)
		{
			glDeleteProgram(id);
			
		}
	}

	Result ShaderProgram::AttachShader(const Shader& shader)
	{
		glAttachShader(id, shader.id);
		CHECK_OPENGL_RESULT();

		return Result();
	}

	Result ShaderProgram::DetachShader(const Shader& shader)
	{
		glDetachShader(id, shader.id);
		CHECK_OPENGL_RESULT();

		return Result();
	}

	Result ShaderProgram::LinkProgram()
	{
		if (state == ShaderProgramState::SuccesfullyLinked)
			BLAZE_WARNING_RESULT("Blaze Engine", "Relinking a shader program. This might cause driver errors. Recreate the shader program before linking again");

		state = ShaderProgramState::UnsuccesfullyLinked;

		glLinkProgram(id);
		CHECK_OPENGL_RESULT();

		int linkStatus = 0;
		glGetProgramiv(id, GL_LINK_STATUS, &linkStatus);
		CHECK_OPENGL_RESULT();

		if (linkStatus == GL_FALSE)
		{
			int lenght = 0;
			glGetProgramiv(id, GL_INFO_LOG_LENGTH, &lenght);
			CHECK_OPENGL_RESULT();

			String message(lenght);

			glGetProgramInfoLog(id, lenght, &lenght, message.Ptr());
			CHECK_OPENGL_RESULT();

			return BLAZE_INFO_RESULT("OpenGL", std::move(message));
		}

		int lenght;
		glGetProgramiv(id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &lenght);
		CHECK_OPENGL_RESULT();

		maxUniformNameLenght = lenght;
		glGetProgramInterfaceiv(id, GL_UNIFORM_BLOCK, GL_MAX_NAME_LENGTH, &lenght);
		CHECK_OPENGL_RESULT();

		maxUniformBlockNameLenght = lenght;

		state = ShaderProgramState::SuccesfullyLinked;

		return Result();
	}

	Result ShaderProgram::LinkShaders(const std::initializer_list<Shader*>& shaders)
	{
		for (auto& s : shaders)
			CHECK_RESULT(AttachShader(*s));

		CHECK_RESULT(LinkProgram());

		for (auto& s : shaders)
			CHECK_RESULT(DetachShader(*s));

		return Result();
	}

	uint ShaderProgram::GetUniformCount() const
	{
		int count = 0;
		glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &count);
		

		return count;
	}

	int ShaderProgram::GetUniformLocation(const StringView& name)
	{
		int location = glGetUniformLocation(id, name.Ptr());
		if (FlushOpenGLResult())
			return -1;

		return location;
	}

	Result ShaderProgram::GetUniformData(uint index, String& name, uint& uniformSize, UniformType& uniformType) const
	{
		char* ptr = new char[maxUniformNameLenght];
		int lenght;
		int size;
		uint type;

		glGetActiveUniform(id, index, maxUniformNameLenght, &lenght, &size, &type, ptr);
		

		name = std::move(String(ptr, maxUniformNameLenght));
		delete[] ptr;
		uniformSize = size;
		uniformType = (UniformType)type;

		return Result();
	}

	uint ShaderProgram::GetUniformBlockCount() const
	{
		int uniformBlockCount;

		glGetProgramInterfaceiv(id, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &uniformBlockCount);
		

		return uniformBlockCount;
	}

	Result ShaderProgram::GetUniformBlockData(uint index, int& location, String& name, uint& uniformBlockSize, std::vector<int>& memberIndicies) const
	{
		char* ptr = new char[maxUniformBlockNameLenght];
		int lenght;

		glGetActiveUniformBlockName(id, index, maxUniformBlockNameLenght, &lenght, ptr);
		CHECK_OPENGL_RESULT();

		name = std::move(String(ptr, lenght));
		delete[] ptr;
		//location = glGetProgramResourceIndex(id, GL_UNIFORM_BLOCK, ptr);

		constexpr const unsigned props1[]{ GL_NUM_ACTIVE_VARIABLES, GL_BUFFER_DATA_SIZE };
		constexpr const unsigned props2[]{ GL_ACTIVE_VARIABLES };

		struct {
			int uniformBlockVariableCount;
			int uniformBlockSize;
		} v1;

		glGetProgramResourceiv(id, GL_UNIFORM_BLOCK, index, 2, props1, 2, &lenght, (int*)&v1);
		CHECK_OPENGL_RESULT();

		memberIndicies.resize(v1.uniformBlockVariableCount);
		glGetProgramResourceiv(id, GL_UNIFORM_BLOCK, index, 1, props2, v1.uniformBlockVariableCount, &lenght, memberIndicies.data());
		CHECK_OPENGL_RESULT();


		uniformBlockSize = v1.uniformBlockSize;
		return Result();
	}

	Result ShaderProgram::GetUniformBlockMemberData(int location, String& name, uint& size, UniformType& type, uint& offset)
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
		CHECK_OPENGL_RESULT();

		char* ptr = new char[v2.uniformBlockVariableNameLenght];

		glGetActiveUniformName(id, location, v2.uniformBlockVariableNameLenght, &lenght, ptr);
		CHECK_OPENGL_RESULT();

		name = std::move(String(ptr, v2.uniformBlockVariableNameLenght));
		delete[] ptr;
		size = v2.uniformBlockVariableSize;
		type = (UniformType)v2.uniformBlockVariableType;
		offset = v2.uniformBlockVariableOffset;
		return Result();
	}

	Result ShaderProgram::BindUniformBlock(int location, uint binding)
	{
		glUniformBlockBinding(id, location, binding); CHECK_OPENGL_RESULT(); return Result();
	}

	Result ShaderProgram::SetUniform(int location, const int& value) const
	{
		glProgramUniform1i(id, location, value); CHECK_OPENGL_RESULT(); return Result();
	}
	Result ShaderProgram::SetUniform(int location, const uint& value) const
	{
		glProgramUniform1ui(id, location, value); CHECK_OPENGL_RESULT(); return Result();
	}
	Result ShaderProgram::SetUniform(int location, const float& value) const
	{
		glProgramUniform1f(id, location, value); CHECK_OPENGL_RESULT(); return Result();
	}
	Result ShaderProgram::SetUniform(int location, const double& value) const
	{
		glProgramUniform1d(id, location, value); CHECK_OPENGL_RESULT(); return Result();
	}
	Result ShaderProgram::SetUniform(int location, const Vec2i& value) const
	{
		glProgramUniform2i(id, location, value.x, value.y); CHECK_OPENGL_RESULT(); return Result();
	}
	Result ShaderProgram::SetUniform(int location, const Vec2f& value) const
	{
		glProgramUniform2f(id, location, value.x, value.y); CHECK_OPENGL_RESULT(); return Result();
	}
	Result ShaderProgram::SetUniform(int location, const Vec2d& value) const
	{
		glProgramUniform2d(id, location, value.x, value.y); CHECK_OPENGL_RESULT(); return Result();
	}
	Result ShaderProgram::SetUniform(int location, const Vec3i& value) const
	{
		glProgramUniform3i(id, location, value.x, value.y, value.z); CHECK_OPENGL_RESULT(); return Result();
	}
	Result ShaderProgram::SetUniform(int location, const Vec3f& value) const
	{
		glProgramUniform3f(id, location, value.x, value.y, value.z); CHECK_OPENGL_RESULT(); return Result();
	}
	Result ShaderProgram::SetUniform(int location, const Vec3d& value) const
	{
		glProgramUniform3d(id, location, value.x, value.y, value.z); CHECK_OPENGL_RESULT(); return Result();
	}
	Result ShaderProgram::SetUniform(int location, const Vec4i& value) const
	{
		glProgramUniform4i(id, location, value.x, value.y, value.z, value.w); CHECK_OPENGL_RESULT(); return Result();
	}
	Result ShaderProgram::SetUniform(int location, const Vec4f& value) const
	{
		glProgramUniform4f(id, location, value.x, value.y, value.z, value.w); CHECK_OPENGL_RESULT(); return Result();
	}
	Result ShaderProgram::SetUniform(int location, const Vec4d& value) const
	{
		glProgramUniform4d(id, location, value.x, value.y, value.z, value.w); CHECK_OPENGL_RESULT(); return Result();
	}
	Result ShaderProgram::SetUniform(int location, const Mat2f& value) const
	{
		glProgramUniformMatrix2fv(id, location, 1, true, (float*)&value); CHECK_OPENGL_RESULT(); return Result();
	}
	Result ShaderProgram::SetUniform(int location, const Mat3f& value) const
	{
		glProgramUniformMatrix3fv(id, location, 1, true, (float*)&value); CHECK_OPENGL_RESULT(); return Result();
	}
	Result ShaderProgram::SetUniform(int location, const Mat4f& value) const
	{
		glProgramUniformMatrix4fv(id, location, 1, true, (float*)&value); CHECK_OPENGL_RESULT(); return Result();
	}

	ShaderProgram& ShaderProgram::operator=(ShaderProgram&& p) noexcept
	{
		if (id != -1)
		{
			glDeleteProgram(id);
			if (FlushOpenGLResult())
				return *this;
		}

		id = p.id;
		p.id = -1;

		state = p.state;
		p.state = ShaderProgramState::Invalid;

		maxUniformBlockNameLenght = p.maxUniformBlockNameLenght;
		p.maxUniformBlockNameLenght = 0;

		maxUniformNameLenght = p.maxUniformNameLenght;
		p.maxUniformNameLenght = 0;

		return *this;
	}
}