#include "pch.h"
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
	}

	Result ResourceTypeRegistry::RegisterType(ResourceTypeData data)
	{		
		auto [it, inserted] = nameTable.Insert(data.typeName, (uint)types.Count());

		if (it.IsNull())
			types.AddBack(std::move(data));
		
		return Result();
	}

	void ResourceTypeRegistry::RegisterCoreTypes()
	{		
	}

	uintMem ResourceTypeRegistry::GetResourceTypeIndex(StringView name) const
	{
		auto it = nameTable.Find(name);

		if (it.IsNull())
			return -1;
		else
			return it->value;
	}

	uintMem ResourceTypeRegistry::GetResourceTypeCount() const
	{
		return static_cast<uint>(types.Count());
	}

	ResourceTypeData ResourceTypeRegistry::GetResourceTypeData(uintMem index) const
	{		
		if (index > types.Count())
		{
			Debug::Logger::LogError("Blaze Engine", "Index is out of bounds. Index was: " + StringParsing::Convert(index));
			return { "" };
		}
		return types[index];
	}

	bool ResourceTypeRegistry::IsValidTypeIndex(uintMem index) const
	{
		return index < types.Count();
	}

	ResourceTypeRegistry ResourceTypeRegistry::CoreRegistry()
	{
		ResourceTypeRegistry registry;
		registry.RegisterCoreTypes();
		return registry;
	}

}