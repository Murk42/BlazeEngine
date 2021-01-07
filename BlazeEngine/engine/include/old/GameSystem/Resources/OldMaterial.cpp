#include "OldMaterial.h"
#include "BlazeEngine/Utilities/File.h"
#include "BlazeEngine/DataStructures/ByteStream.h"
#include "BlazeEngine/Core/Logger.h"

namespace Blaze
{	
	static inline void* AllocateProperties(ShaderProgram& sp)
	{
		uint size = 0;
		const auto& uniforms = sp.GetUniforms();
		for (const auto& u : uniforms)
			if (u.IsSampler())
				size += sizeof(Texture*);
			else
				size += SizeOf(u.GetType());

		void* ptr = new uint8[size];
		memset(ptr, 0, size);
		return ptr;
	}
	static inline void DeallocateProperties(void* ptr)
	{
		delete[] ptr;
	}
	static inline void* GetPropertyValue(ShaderProgram& sp, void* ptr, uint index)
	{
		uint offset = 0;
		const auto& uniforms = sp.GetUniforms();
		for (uint i = 0; i < index; ++i)
			if (uniforms[i].IsSampler())
				offset += sizeof(Texture*);
			else
				offset += SizeOf(uniforms[i].GetType());

		return (uint8*)ptr + offset;
	}

	void OldMaterial::_SetProperty(PropertyID id, const void* value)
	{
		const auto& uniforms = shaderProgram.GetUniforms();

		if (id < 0 && id >= uniforms.size())
		{
			Logger::AddLog(LogType::Error, "OldMaterial::SetProperty", "Invalid property id");
			return;
		}

		void* property = GetPropertyValue(shaderProgram, propertyValues, id);		

		if (uniforms[id].IsSampler())
			memcpy(property, &value, sizeof(Texture*));
		else
			memcpy(property, value, SizeOf(uniforms[id].GetType()));			
	}		

	OldMaterial::OldMaterial()
	{

	}
	OldMaterial::OldMaterial(const OldMaterial& m)
		: shaderProgram(m.shaderProgram)
	{

	}
	OldMaterial::OldMaterial(OldMaterial&& m)
		: shaderProgram(std::move(m.shaderProgram))
	{
	}
	OldMaterial::OldMaterial(const std::initializer_list<const Shader*>& shaders)
	{
		SetShaders(shaders);
	}
	OldMaterial::~OldMaterial()
	{
		DeallocateProperties(propertyValues);
	}

	uint OldMaterial::GetPropertyID(StringView name)
	{
		const auto& uniforms = shaderProgram.GetUniforms();
		for (uint i = 0; i < uniforms.size(); ++i)
			if (uniforms[i].GetName() == name)
				return i;
		return -1;
	}

	void OldMaterial::SetShaders(const std::initializer_list<const Shader*>& shaders)
	{ 
		shaderProgram.SetShaders(shaders);		
		propertyValues = AllocateProperties(shaderProgram);		
	}

	void OldMaterial::UpdateUniforms()
	{		
		const auto& uniforms = shaderProgram.GetUniforms();

		uint offset = 0;
		uint texCount = 0;
		for (uint i = 0; i < uniforms.size(); ++i)
			if (uniforms[i].IsSampler())
			{
				shaderProgram.SetUniform(&uniforms[i], &texCount);				

				texCount++;
				offset += sizeof(Texture*);
			}
			else
			{
				shaderProgram.SetUniform(&uniforms[i], (uint8*)propertyValues + offset);
				offset += SizeOf(uniforms[i].GetType());
			}
	}	
	void OldMaterial::BindTextures()
	{
		const auto& uniforms = shaderProgram.GetUniforms();

		uint offset = 0;
		uint texCount = 0;
		for (uint i = 0; i < uniforms.size(); ++i)
			if (uniforms[i].IsSampler())
			{
				(*(Texture**)((uint8*)propertyValues + offset))->Bind(texCount);				

				texCount++;
				offset += sizeof(Texture*);
			}
			else
				offset += SizeOf(uniforms[i].GetType());
	}

	void OldMaterial::operator=(const OldMaterial& m)
	{
		shaderProgram = m.shaderProgram;
	}
	void OldMaterial::operator=(OldMaterial&& m)
	{
		shaderProgram = std::move(m.shaderProgram);		
	}
}