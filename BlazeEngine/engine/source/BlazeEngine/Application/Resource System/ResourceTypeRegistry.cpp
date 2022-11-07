#include "BlazeEngine/Application/ResourceSystem/ResourceTypeRegistry.h"
#include "BlazeEngine/Logging/Logger.h"
#include "BlazeEngine/Resources/Font/Font.h"

namespace Blaze::ResourceSystem
{
	Result ResourceTypeRegistry::RegisterType(StringView name, size_t size, void(*construct)(void*), void(*destruct)(void*))
	{
		ResourceTypeData data;
		data.name = name;
		data.size = size;
		data.construct = construct;
		data.destruct = destruct;		

		if (nameTable.try_emplace(name, types.size()).second)
			types.emplace_back(std::move(data));

		return Result();
	}

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

	void ResourceTypeRegistry::RegisterCoreTypes()
	{
		RegisterType<Font>();
		RegisterType<Graphics::Core::Texture2D>();
		//RegisterType<FontResolution>();
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
		return types.size();
	}

	const ResourceTypeData& ResourceTypeRegistry::GetResourceTypeData(uint index) const
	{
		if (index > types.size())
		{
			Logger::AddLog(LogType::Error, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE,
				"Blaze Engine", "Index is out of bounds. Index was: " + String::Convert(index));
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