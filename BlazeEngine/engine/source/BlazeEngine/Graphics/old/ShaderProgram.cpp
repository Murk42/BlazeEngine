#include "BlazeEngine/Graphics/ShaderProgram.h"
#include "BlazeEngine/Graphics/Shader.h"
#include "BlazeEngine/Core/Logger.h"
#include "BlazeEngine/Graphics/Buffer.h"
#include "BlazeEngine/DataStructures/Guard.h"
#include "BlazeEngine/DataStructures/ByteStream.h"

#include "source/BlazeEngine/Internal/Engine.h"

#include "GL/glew.h"
#include <unordered_map>

namespace Blaze
{
	static inline bool IsSampler(unsigned type)
	{
		if (type == GL_SAMPLER_1D
			|| type == GL_SAMPLER_2D
			|| type == GL_SAMPLER_3D
			|| type == GL_SAMPLER_CUBE
			|| type == GL_SAMPLER_2D_RECT
			|| type == GL_SAMPLER_1D_ARRAY
			|| type == GL_SAMPLER_2D_ARRAY
			|| type == GL_SAMPLER_CUBE_MAP_ARRAY
			|| type == GL_SAMPLER_BUFFER
			|| type == GL_SAMPLER_2D_MULTISAMPLE
			|| type == GL_SAMPLER_2D_MULTISAMPLE_ARRAY)
			return true;
		return false;
	}

	Uniform::Uniform()
		: type((Type)0), pos(0), count(0), sampler(false)
	{
	}

	Uniform::Uniform(Uniform&& u) noexcept
		: name(std::move(u.name)), type(u.type), pos(u.pos), count(u.count), sampler(u.sampler)
	{

	}
	Uniform::~Uniform()
	{
   	}	

	UniformBlock::UniformBlock()
		: index(0), size(0), data(nullptr), buffer(GraphicsBufferType::UniformBuffer)
	{
	}

	UniformBlock::UniformBlock(UniformBlock&& u) noexcept
		: name(std::move(u.name)), index(u.index), size(u.size), members(std::move(u.members)), data(std::exchange(u.data, nullptr)), buffer(std::move(u.buffer))
	{
	}
	UniformBlock::~UniformBlock()
	{		
		delete data;		
	}
	
	void ShaderProgram::GatherUniformData()
	{
		//Get uniform data
		{
			int uniformCount, uniformMaxNameLenght;
			glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &uniformCount);
			glGetProgramiv(id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformMaxNameLenght);

			char* uniformName = new char[uniformMaxNameLenght];
			int uniformNameLenght;
			int uniformSize;
			int uniformLocation;			
			unsigned uniformType;

			uniforms.resize(uniformCount);
			for (int i = 0; i < uniformCount; i++)
			{
				memset(uniformName, 0, uniformMaxNameLenght);

				glGetActiveUniform(id, i, uniformMaxNameLenght, &uniformNameLenght, &uniformSize, &uniformType, uniformName);
				uniformLocation = glGetUniformLocation(id, uniformName);

				uniforms[i].name = String(uniformName, (size_t)uniformNameLenght);
				uniforms[i].pos = uniformLocation;
				uniforms[i].count = uniformSize;
				uniforms[i].sampler = IsSampler(uniformType);
				uniforms[i].type = (uniforms[i].sampler ? Type::Int : (Type)uniformType);
			}
			delete[] uniformName;
		}

		//Get uniform block data
		{
			int uniformBlockCount, uniformBlockMaxNameLenght;
			glGetProgramInterfaceiv(id, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &uniformBlockCount);
			glGetProgramInterfaceiv(id, GL_UNIFORM_BLOCK, GL_MAX_NAME_LENGTH, &uniformBlockMaxNameLenght);

			int lenghtBuffer;
			constexpr const unsigned props1[]{ GL_NUM_ACTIVE_VARIABLES, GL_BUFFER_DATA_SIZE };
			constexpr const unsigned props2[]{ GL_ACTIVE_VARIABLES };
			constexpr const unsigned props3[]{ GL_NAME_LENGTH, GL_OFFSET, GL_TYPE, GL_ARRAY_SIZE };

			engine->Renderer.uniformBlocks.resize(uniformBlockCount);

			char* uniformBlockName = new char[uniformBlockMaxNameLenght];
			int uniformBlockNameLenght;
			int uniformBlockIndex;
			int* uniformBlockVariableIndices;
			for (int i = 0; i < uniformBlockCount; i++)
			{
				struct {
					int uniformBlockVariableCount;
					int uniformBlockSize;
				} v1;

				memset(uniformBlockName, 0, uniformBlockMaxNameLenght);
				glGetActiveUniformBlockName(id, i, uniformBlockMaxNameLenght, &uniformBlockNameLenght, uniformBlockName);
				uniformBlockIndex = glGetProgramResourceIndex(id, GL_UNIFORM_BLOCK, uniformBlockName);


				glGetProgramResourceiv(id, GL_UNIFORM_BLOCK, uniformBlockIndex, 2, props1, 2, &lenghtBuffer, (int*)&v1);
				engine->Renderer.uniformBlocks[i].members.resize(v1.uniformBlockVariableCount);
				uniformBlockVariableIndices = new int[v1.uniformBlockVariableCount];
				glGetProgramResourceiv(id, GL_UNIFORM_BLOCK, uniformBlockIndex, 1, props2, v1.uniformBlockVariableCount, &lenghtBuffer, uniformBlockVariableIndices);

				engine->Renderer.uniformBlocks[i].name = String(uniformBlockName, (size_t)uniformBlockNameLenght);
				engine->Renderer.uniformBlocks[i].size = v1.uniformBlockSize;
				engine->Renderer.uniformBlocks[i].index = uniformBlockIndex;

				for (int j = 0; j < v1.uniformBlockVariableCount; j++)
				{
					struct {
						int uniformBlockVariableNameLenght;
						int uniformBlockVariableOfbset;
						int uniformBlockVariableType;
						int uniformBlockVariableSize;
					} v2;
					char* uniformBlockVariableName;

					glGetProgramResourceiv(id, GL_UNIFORM, uniformBlockVariableIndices[j], 4, props3, 4, &lenghtBuffer, (int*)&v2);
					uniformBlockVariableName = new char[v2.uniformBlockVariableNameLenght];
					glGetActiveUniformName(id, uniformBlockVariableIndices[j], v2.uniformBlockVariableNameLenght, &lenghtBuffer, uniformBlockVariableName);

					engine->Renderer.uniformBlocks[i].members[j].offset = v2.uniformBlockVariableOfbset;
					engine->Renderer.uniformBlocks[i].members[j].u.name = uniformBlockVariableName;
					engine->Renderer.uniformBlocks[i].members[j].u.pos = uniformBlockVariableIndices[j];
					engine->Renderer.uniformBlocks[i].members[j].u.count = v2.uniformBlockVariableSize;
					engine->Renderer.uniformBlocks[i].members[j].u.sampler = (v2.uniformBlockVariableType == GL_SAMPLER_2D);
					engine->Renderer.uniformBlocks[i].members[j].u.type = (v2.uniformBlockVariableType == GL_SAMPLER_2D ? Type::Int : (Type)v2.uniformBlockVariableType);

					delete[] uniformBlockVariableName;
				}
				delete[] uniformBlockVariableIndices;
			}
			delete[] uniformBlockName;
		}

		//Bind uniform block buffers
		{
			for (int i = 0; i < engine->Renderer.uniformBlocks.size(); i++)
			{
				UniformBlock& u = engine->Renderer.uniformBlocks[i];
				u.data = new byte[u.size];
				memset(u.data, 0, u.size);
				u.buffer.AllocateData(u.data, u.size, BufferUsage::Dynamic | BufferUsage::Draw);
				glUniformBlockBinding(id, u.index, i);
				glBindBufferBase(GL_UNIFORM_BUFFER, i, u.buffer.id);
			}
		}
	}

	ShaderProgram::ShaderProgram()
	{
		id = glCreateProgram();		
	}
	ShaderProgram::ShaderProgram(const ShaderProgram& sp)
	{
		id = glCreateProgram();

		int binarySize;
		glGetProgramiv(sp.id, GL_PROGRAM_BINARY_LENGTH, &binarySize);

		uint format;
		Guard<void, true> binary = new uint8[binarySize];
		glGetProgramBinary(sp.id, binarySize, nullptr, &format, binary.ptr);
				
		glProgramBinary(id, format, binary.ptr, binarySize);
	}
	ShaderProgram::ShaderProgram(ShaderProgram&& sp) noexcept
		: id(std::exchange(sp.id, 0)), uniforms(std::move(sp.uniforms))
	{
	}
	ShaderProgram::~ShaderProgram()
	{
		glDeleteProgram(id);
	}


	bool ShaderProgram::SetShaders(const std::initializer_list<const Shader*>& shaders)
	{		
		for (auto& shader : shaders)
			glAttachShader(id, shader->id);

		glLinkProgram(id);

		int linkStatus = 0;
		glGetProgramiv(id, GL_LINK_STATUS, &linkStatus);
		if (linkStatus == GL_FALSE)
		{
			int logLength = 0;
			glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);

			Guard<char, true> log = new char[logLength];
			
			glGetProgramInfoLog(id, logLength, &logLength, log.ptr);

			Logger::AddLog(LogType::Error, "ShaderProgram::SetShaders", StringView(log.ptr, logLength));

			return false;
		}		

		for (auto& shader : shaders)
			glDetachShader(id, shader->id);

		GatherUniformData();
		return true;
	}

	const Uniform* ShaderProgram::GetUniform(const String& name) const
	{
		for (auto& uniform : uniforms)		
			if (uniform.name == name)
				return (Uniform*)&uniform;		
		return nullptr;		
	}
	UniformBlock* ShaderProgram::GetUniformBlock(const String& name)
	{
		for (auto& uniform : engine->Renderer.uniformBlocks)
			if (uniform.name == name)
				return (UniformBlock*)&uniform;
		return nullptr;
	}

	const std::vector<UniformBlock>& ShaderProgram::GetUniformBlocks()
	{
		return engine->Renderer.uniformBlocks;
	}

	void ShaderProgram::SetUniform(const Uniform* uniform, const void* ptr) const
	{	
#ifdef _DEBUG
		float* pf = (float*)ptr;
		int* pi = (int*)ptr;
#endif
		switch (uniform->type)
		{
		case Type::Int:   glProgramUniform1iv(id, uniform->pos, uniform->count, (const int*)ptr);  break;
		case Type::Float: glProgramUniform1fv(id, uniform->pos, uniform->count, (const float*)ptr); break;
		case Type::Vec2i: glProgramUniform2iv(id, uniform->pos, uniform->count, (const int*)ptr); break;
		case Type::Vec2f: glProgramUniform2fv(id, uniform->pos, uniform->count, (const float*)ptr); break;
		case Type::Vec3i: glProgramUniform3iv(id, uniform->pos, uniform->count, (const int*)ptr); break;
		case Type::Vec3f: glProgramUniform3fv(id, uniform->pos, uniform->count, (const float*)ptr); break;
		case Type::Vec4i: glProgramUniform4iv(id, uniform->pos, uniform->count, (const int*)ptr); break;
		case Type::Vec4f: glProgramUniform4fv(id, uniform->pos, uniform->count, (const float*)ptr); break;
		case Type::Mat2f: glProgramUniformMatrix2fv(id, uniform->pos, uniform->count, GL_TRUE, (const float*)ptr); break;
		case Type::Mat3f: glProgramUniformMatrix3fv(id, uniform->pos, uniform->count, GL_TRUE, (const float*)ptr); break;
		case Type::Mat4f: glProgramUniformMatrix4fv(id, uniform->pos, uniform->count, GL_TRUE, (const float*)ptr); break;
		}
	}
	bool ShaderProgram::SetUniform(StringView name, const void* ptr) const
	{		
		const Uniform* uniform = GetUniform(name);
		if (uniform != nullptr)
		{
			SetUniform(uniform, ptr);
			return true;
		}
		else
			Logger::AddLog(LogType::Warning, "ShaderProgram::SetUniform", "Trying to set a uniform that doesnt exist!");
		return false;
	}

	void ShaderProgram::Serialize(ByteStream& bs)
	{
		int lenght;
		glGetProgramiv(id, GL_PROGRAM_BINARY_LENGTH, &lenght);

		uint format;
		Guard<void, true> binary = new uint8[lenght];
		glGetProgramBinary(id, lenght, nullptr, &format, binary.ptr);
		bs.Set<uint32>(format);
		bs.Set<uint32>(lenght);
		bs.Set(binary.ptr, lenght);		
	}
	void ShaderProgram::Deserialize(ByteStream& bs)
	{
		uint format = bs.Get<uint32>();
		int lenght = bs.Get<uint32>();
		Guard<void, true> binary = new uint8[lenght];
		bs.Get(binary.ptr, lenght);
		glProgramBinary(id, format, binary.ptr, lenght);

		GatherUniformData();
	}
	 
	void ShaderProgram::SetUniformBlock(const UniformBlock* data, const void* ptr)
	{	
		for (int i = 0; i < data->members.size(); i++)
		{
			memcpy((byte*)data->data + data->members[i].offset, (byte*)ptr + offset, SizeOf(data->members[i].u.type));
			offset += SizeOf(data->members[i].u.type);
		}

		((Buffer&)data->buffer).ChangeData(data->data, data->size, 0);
	}
	bool ShaderProgram::SetUniformBlock(const String& name, const void* ptr)
	{
		UniformBlock* uniform = GetUniformBlock(name);
		if (uniform != nullptr)
		{
			SetUniformBlock(uniform, ptr);
			return true;
		}
		else
			Logger::AddLog(LogType::Warning, "ShaderProgram::SetUniformBlock", "Trying to set a uniform that doesnt exist!");
		return false;
	}

	void ShaderProgram::operator=(const ShaderProgram& sp)
	{
		int binarySize;
		glGetProgramiv(sp.id, GL_PROGRAM_BINARY_LENGTH, &binarySize);

		uint format;
		Guard<void, true> binary = new uint8[binarySize];
		glGetProgramBinary(sp.id, binarySize, nullptr, &format, binary.ptr);

		glProgramBinary(id, format, binary.ptr, binarySize);
	}
	void ShaderProgram::operator=(ShaderProgram&& sp) noexcept
	{
		glDeleteProgram(id);
		id = std::exchange(sp.id, 0);
		uniforms = std::move(sp.uniforms);
	}

	void ShaderProgram::Bind() const
	{
		glUseProgram(id);
		engine->Renderer.boundShaderProgram = (ShaderProgram*)this;
	}
	void ShaderProgram::Unbind()
	{
		glUseProgram(0);
		engine->Renderer.boundShaderProgram = nullptr;
	}
	ShaderProgram* ShaderProgram::GetBound()
	{
		return engine->Renderer.boundShaderProgram;
	}
}