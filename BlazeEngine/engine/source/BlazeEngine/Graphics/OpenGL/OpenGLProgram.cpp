#include "BlazeEngine/Graphics/OpenGL/OpenGLProgram.h"
#include "BlazeEngine/DataStructures/String.h"
#include "GL/glew.h"
#include "BlazeEngine/Logging/LogListener.h"

namespace Blaze
{
	namespace OpenGL
	{
		ShaderProgram::ShaderProgram()
			: id(-1), state(ShaderProgramState::Invalid), maxUniformBlockNameLenght(0), maxUniformNameLenght(0)
		{
			id = glCreateProgram();
		}
		ShaderProgram::ShaderProgram(ShaderProgram&& p) noexcept
			: state(p.state), maxUniformBlockNameLenght(p.maxUniformBlockNameLenght), maxUniformNameLenght(p.maxUniformNameLenght)
		{			
			p.id = -1;
			p.state = ShaderProgramState::Invalid;
			p.maxUniformBlockNameLenght = 0;
			p.maxUniformNameLenght = 0;
		}
		ShaderProgram::~ShaderProgram()
		{
			if (id != -1)
				glDeleteProgram(id);
		}

		void ShaderProgram::AttachShader(const Shader& shader)
		{
			glAttachShader(id, shader.id);
		}

		void ShaderProgram::DetachShader(const Shader& shader)
		{
			glDetachShader(id, shader.id);
		}

		Result ShaderProgram::LinkProgram()
		{
			LogListener listener;
			listener.SupressLogs(true);
			listener.StartListening();

			if (state == ShaderProgramState::Valid)
				Logger::AddLog(BLAZE_WARNING_LOG("Blaze Engine", "Relinking a shader program. This might cause driver errors. Recreate the shader program before linking again"));

			glLinkProgram(id);

			int linkStatus = 0;
			glGetProgramiv(id, GL_LINK_STATUS, &linkStatus);
			listener.StopListening();

			Result result;
			result.AddLogs(listener.GetLogs());

			if (linkStatus == GL_FALSE)
			{
				int lenght = 0;
				glGetProgramiv(id, GL_INFO_LOG_LENGTH, &lenght);

				String message(lenght);

				glGetProgramInfoLog(id, lenght, &lenght, message.Ptr());

				result.AddLog(BLAZE_INFO_LOG("OpenGL", std::move(message)));
				result.SetFailed(true);

				state = ShaderProgramState::Invalid;
				return result;
			}

			int lenght;
			glGetProgramiv(id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &lenght);
			maxUniformNameLenght = lenght;
			glGetProgramInterfaceiv(id, GL_UNIFORM_BLOCK, GL_MAX_NAME_LENGTH, &lenght);
			maxUniformBlockNameLenght = lenght;

			state = ShaderProgramState::Valid;

			return result;
		}		

		Result ShaderProgram::LinkShaders(const std::initializer_list<Shader*>& shaders)
		{
			for (auto& s : shaders)
				AttachShader(*s);			

			if (Result result = LinkProgram())
				return result;

			for (auto& s : shaders)
				DetachShader(*s);

			return Result();
		}

		uint ShaderProgram::GetUniformCount() const
		{
			int count;
			glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &count);
			return count;
		}		

		int ShaderProgram::GetUniformLocation(const StringView& name)
		{
			return glGetUniformLocation(id, name.Ptr());
		}

		void ShaderProgram::GetUniformData(uint index, String& name, uint& uniformSize, UniformType& uniformType) const
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

		uint ShaderProgram::GetUniformBlockCount() const
		{		
			int uniformBlockCount;
			glGetProgramInterfaceiv(id, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &uniformBlockCount);
			return uniformBlockCount;
		}

		void ShaderProgram::GetUniformBlockData(uint index, int& location, String& name, uint& uniformBlockSize, std::vector<int>& memberIndicies) const
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
			type = (UniformType)v2.uniformBlockVariableType;
			offset = v2.uniformBlockVariableOffset;
		}
		
		void ShaderProgram::SetUniform(int location, const bool& value)
		{ 						
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
		void ShaderProgram::SetUniform(int location, const Vec2i& value)
		{
			glProgramUniform2i(id, location, value.x, value.y);
		}		
		void ShaderProgram::SetUniform(int location, const Vec2f& value)
		{
			glProgramUniform2f(id, location, value.x, value.y);
		}		
		void ShaderProgram::SetUniform(int location, const Vec3i& value)
		{
			glProgramUniform3i(id, location, value.x, value.y, value.z);
		}		
		void ShaderProgram::SetUniform(int location, const Vec3f& value)
		{
			glProgramUniform3f(id, location, value.x, value.y, value.z);
		}
		void ShaderProgram::SetUniform(int location, const Vec4f& value)
		{
			glProgramUniform4f(id, location, value.x, value.y, value.z, value.w);
		}		
		void ShaderProgram::SetUniform(int location, const Mat2f& value)
		{
			glProgramUniformMatrix2fv(id, location, 1, true, value.arr);
		}		
		void ShaderProgram::SetUniform(int location, const Mat3f& value)
		{
			glProgramUniformMatrix3fv(id, location, 1, true, value.arr);
		}		
		void ShaderProgram::SetUniform(int location, const Mat4f& value)
		{
			glProgramUniformMatrix4fv(id, location, 1, true, value.arr);
		}
				
		ShaderProgram& ShaderProgram::operator=(ShaderProgram&& p) noexcept
		{
			if (id != -1)
				glDeleteProgram(id);

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
}