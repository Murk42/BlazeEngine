#include "BlazeEngine/Graphics/OpenGL/OpenGLProgram.h"
#include "BlazeEngine/DataStructures/String.h"

#include "source/BlazeEngine/Internal/Engine.h"

#include "GL/glew.h"

namespace Blaze
{
	namespace OpenGL
	{
		Program::Program()
			: id(-1)
		{
			id = glCreateProgram();
		}
		Program::Program(Program&& p) noexcept
			: id(-1)
		{
			id = p.id;
			p.id = -1;
		}
		Program::~Program()
		{
			if (id != -1)
				glDeleteProgram(id);
		}

		void Program::AttachShader(const Shader& shader)
		{
			glAttachShader(id, shader.id);
		}

		void Program::DetachShader(const Shader& shader)
		{
			glDetachShader(id, shader.id);
		}

		bool Program::LinkProgram()
		{
			glLinkProgram(id);

			int linkStatus = 0;
			glGetProgramiv(id, GL_LINK_STATUS, &linkStatus);

			if (linkStatus == GL_FALSE)
				return BLAZE_ERROR;

			int lenght;
			glGetProgramiv(id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &lenght);
			maxUniformNameLenght = lenght;
			glGetProgramInterfaceiv(id, GL_UNIFORM_BLOCK, GL_MAX_NAME_LENGTH, &lenght);
			maxUniformBlockNameLenght = lenght;

			return BLAZE_OK;
		}

		String Program::GetLinkingLog()
		{
			int lenght = 0;
			glGetProgramiv(id, GL_INFO_LOG_LENGTH, &lenght);
			String log(lenght);
			glGetProgramInfoLog(id, lenght, &lenght, log.Ptr());
			return log;
		}

		uint Program::GetUniformCount() const
		{
			int count;
			glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &count);
			return count;
		}		

		int Program::GetUniformLocation(const StringView& name)
		{
			return glGetUniformLocation(id, name.Ptr());
		}

		void Program::GetUniformData(uint index, String& name, uint& uniformSize, UniformType& uniformType) const
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
		}

		uint Program::GetUniformBlockCount() const
		{		
			int uniformBlockCount;
			glGetProgramInterfaceiv(id, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &uniformBlockCount);
			return uniformBlockCount;
		}

		void Program::GetUniformBlockData(uint index, int& location, String& name, uint& uniformBlockSize, std::vector<int>& memberIndicies) const
		{
			char* ptr = new char[maxUniformBlockNameLenght];
			int lenght;
			glGetActiveUniformBlockName(id, index, maxUniformBlockNameLenght, &lenght, ptr);
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
			memberIndicies.resize(v1.uniformBlockVariableCount);
			glGetProgramResourceiv(id, GL_UNIFORM_BLOCK, index, 1, props2, v1.uniformBlockVariableCount, &lenght, memberIndicies.data());

			uniformBlockSize = v1.uniformBlockSize;
		}

		void Program::GetUniformBlockMemberData(int location, String& name, uint& size, UniformType& type, uint& offset)
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
			type = (UniformType)v2.uniformBlockVariableType;
			offset = v2.uniformBlockVariableOffset;
		}

		template<>
		void Program::SetUniform(int location, const int& value)
		{
			glProgramUniform1i(id, location, value);
		}
		template<>
		void Program::SetUniform(int location, const float& value)
		{
			glProgramUniform1f(id, location, value);
		}
		template<>
		void Program::SetUniform(int location, const Vec3f& value)
		{
			glProgramUniform3f(id, location, value.x, value.y, value.z);
		}
		template<>
		void Program::SetUniform(int location, const Vec4f& value)
		{
			glProgramUniform4f(id, location, value.x, value.y, value.z, value.w);
		}
		template<>
		void Program::SetUniform(int location, const Mat4f& value)
		{
			glProgramUniformMatrix4fv(id, location, 1, true, value.arr);
		}
				
		Program& Program::operator=(Program&& p) noexcept
		{
			if (id != -1)
				glDeleteProgram(id);
			id = p.id;
			p.id = -1;

			return *this;
		}
	}
}