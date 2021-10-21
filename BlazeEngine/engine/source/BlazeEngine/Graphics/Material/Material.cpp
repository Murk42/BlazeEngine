#include "BlazeEngine/Graphics/Material/Material.h"
#include "source/BlazeEngine/Internal/Engine.h"

namespace Blaze
{
	Material::PropertyBase::PropertyBase(const char* name)
	{
		Material*& mat = engine->Renderer.initializingMaterial;

		if (mat->propertyCount == maxProperties)
			throw "Property count limit of " + String::Convert(maxProperties) + " properties exceded";

		mat->properties[mat->propertyCount] = this;
		++mat->propertyCount;

		this->name = name;
	}	

	Material::Material()
		: program(nullptr), propertyCount(0)
	{
		engine->Renderer.initializingMaterial = this;
	}

	void Material::SetProgram(Graphics::Core::Program* program)
	{
		this->program = program;

		uint uniformCount = program->GetUniformCount();

		if (uniformCount != propertyCount)
			BLAZE_WARNING_LOG("Blaze Engine", "The number of program uniforms and properties isnt the same");

		for (int i = 0; i < uniformCount; ++i)
		{
			String name;
			uint size;
			Graphics::Core::UniformType type;

			program->GetUniformData(i, name, size, type);

			for (int j = 0; j < propertyCount; ++j)
				if (name == properties[j]->name)
				{
					properties[j]->location = program->GetUniformLocation(name);
					if (properties[j]->location == -1)
					{
						BLAZE_ERROR_LOG("Blaze Engine", "The program you set doesn't have the materials property");
						break;
					}
					properties[j]->type = type;
					break;
				}
		}
	}

	void Material::UpdateProperties()
	{
		if (program == nullptr)
		{
			BLAZE_WARNING_LOG("Blaze Engine", "program is nullptr");
			return;
		}

		int samplerIndex = 0;
		for (int i = 0; i < propertyCount; ++i)
		{
			if (properties[i]->type == Graphics::Core::UniformType::Sampler2D)
				++samplerIndex;
			properties[i]->SetProperty(program, samplerIndex);
		}
	}
}