#include "pch.h"
#include "BlazeEngine/Application/ResourceSystem/ResourceManager.h"

namespace Blaze::ResourceSystem
{
	ResourceManager::ResourceManager()
		: resources(nullptr), resourceMap(nullptr), resourceToNameMap(nullptr), typeCount(0)
	{
	}

	ResourceManager::~ResourceManager()
	{		
		delete[] resources;
		delete[] resourceMap;
		delete[] resourceToNameMap;
	}

	void ResourceManager::SetResourceTypeRegistry(const ResourceTypeRegistry& registry)
	{		
		delete[] resources;
		delete[] resourceMap;
		delete[] resourceToNameMap;

		this->registry = registry;
		typeCount = registry.GetResourceTypeCount();

		resources = new Array<Resource*>[typeCount];
		resourceMap = new Map<String, Resource*>[typeCount];
		resourceToNameMap = new Map<Resource* , String>[typeCount];
	}

	Result ResourceManager::AddResource(StringView name, Resource* resource, uintMem typeIndex)
	{
		if (!registry.IsValidTypeIndex(typeIndex))
			return BLAZE_ERROR_RESULT("Blaze Engine", "Invalid type index");

		resources[typeIndex].AddBack(resource);
		resourceMap[typeIndex].Insert(name, resource);
		resourceToNameMap[typeIndex].Insert(resource, name);

		return Result();
	}

	Result ResourceManager::AddResource(Resource* resource, uintMem typeIndex)
	{
		if (!registry.IsValidTypeIndex(typeIndex))
			return BLAZE_ERROR_RESULT("Blaze Engine", "Invalid type index");

		resources[typeIndex].AddBack(resource);

		return Result();
	}

	Result ResourceManager::RemoveResource(Resource* resource, uintMem typeIndex)
	{
		resourceToNameMap->Erase(resource);
		resourceMap[typeIndex].Erase(resource->GetName());				
				
		auto& arr = resources[typeIndex];
		auto it = arr.FirstIterator();
		for (; it != arr.LastIterator(); ++it)
			if (*it == resource)
			{
				arr.EraseAt(it);
				break;
			}					

		return Result();
	}

	Result ResourceManager::RemoveResource(StringView name, uintMem typeIndex)
	{
		Resource* resource;
		auto it = resourceMap[typeIndex].Find(name);

		if (!it.IsNull())
		{
			resourceMap[typeIndex].Erase(it);
			resourceToNameMap->Erase(it->value);
			resource = it->value;
		}
		else
			return Result();

		auto& arr = resources[typeIndex];
		for (auto it = arr.FirstIterator(); it != arr.LastIterator(); ++it)
			if (*it == resource)
			{
				arr.EraseAt(it);
				break;
			}

		return Result();
	}

	Resource* ResourceManager::GetResource(StringView name, uintMem typeIndex)
	{		
		auto& map = resourceMap[typeIndex];
		auto it = map.Find(name);
		if (it.IsNull())
			return nullptr;
		return
			it->value;
	}

	Resource* ResourceManager::GetResource(uintMem index, size_t typeIndex)
	{
		return resources[typeIndex][index];
	}

	String ResourceManager::GetResourceName(Resource* resource, uintMem typeIndex)
	{
		auto& map = resourceToNameMap[typeIndex];
		auto it = map.Find(resource);
		if (it.IsNull())
			return "";
		else
			return it->value;
	}

	bool ResourceManager::HasResource(StringView name, uintMem typeIndex)
	{
		auto it = resourceMap[typeIndex].Find(name);
		return !it.IsNull();
	}

	uintMem ResourceManager::GetResourceCount(uintMem typeIndex)
	{ 
		return resources[typeIndex].Count();
	}

}