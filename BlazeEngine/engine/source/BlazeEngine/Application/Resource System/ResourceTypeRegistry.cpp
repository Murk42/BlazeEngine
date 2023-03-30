#pragma once
#include "BlazeEngine/Application/ResourceSystem/ResourceTypeRegistry.h"
#include "BlazeEngine/Resources/Font/Font.h"

namespace Blaze::ResourceSystem
{	
	ResourceTypeRegistry::ResourceTypeRegistry()
	{
	}

	ResourceTypeRegistry::ResourceTypeRegistry(const ResourceTypeRegistry& other)
		: types(other.types), nameTable(other.nameTable)
	{
	}

	ResourceTypeRegistry::~ResourceTypeRegistry()
	{
		nameTable.clear();
	}

	Result ResourceTypeRegistry::RegisterType(ResourceTypeData data)
	{		
		if (nameTable.try_emplace(data.typeName, types.size()).second)
			types.emplace_back(std::move(data));
		
		return Result();
	}

	void ResourceTypeRegistry::RegisterCoreTypes()
	{
		RegisterType<Font>();
		RegisterType<Graphics::Core::Texture2D>();
		RegisterType<FontResolution>();
	}

	uint ResourceTypeRegistry::GetResourceTypeIndex(StringView name) const
	{
		auto it = nameTable.find((String)name);
		if (it == nameTable.end())
			return -1;
		else
			return it->second;
	}

	uint ResourceTypeRegistry::GetResourceTypeCount() const
	{
		return static_cast<uint>(types.size());
	}

	ResourceTypeData ResourceTypeRegistry::GetResourceTypeData(uint index) const
	{
		if (index > types.size())
		{
			Logger::AddLog(LogType::Error, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE,
				"Blaze Engine", "Index is out of bounds. Index was: " + StringParsing::Convert(index).value);
			return { "" };
		}
		return types[index];
	}

	bool ResourceTypeRegistry::IsValidTypeIndex(uint index) const
	{
		return index < types.size();
	}

	ResourceTypeRegistry ResourceTypeRegistry::CoreRegistry()
	{
		ResourceTypeRegistry registry;
		registry.RegisterCoreTypes();
		return registry;
	}

}