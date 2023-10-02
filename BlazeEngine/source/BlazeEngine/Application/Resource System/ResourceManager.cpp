#include "BlazeEngine/Application/ResourceSystem/ResourceManager.h"

namespace Blaze::ResourceSystem
{
	ResourceManager::ResourceManager()
		: resources(nullptr), resourceMap(nullptr), resourceToNameMap(nullptr), typeCount(0)
	{
	}

	ResourceManager::~ResourceManager()
	{
		for (size_t i = 0; i < typeCount; ++i)
			resourceMap[i].clear();
		delete[] resources;
		delete[] resourceMap;
		delete[] resourceToNameMap;
	}

	void ResourceManager::SetResourceTypeRegistry(const ResourceTypeRegistry& registry)
	{
		for (size_t i = 0; i < typeCount; ++i)
			resourceMap[i].clear();
		delete[] resources;
		delete[] resourceMap;
		delete[] resourceToNameMap;

		this->registry = registry;
		typeCount = registry.GetResourceTypeCount();

		resources = new std::vector<Resource*>[typeCount];
		resourceMap = new std::unordered_map<String, Resource*>[typeCount];
		resourceToNameMap = new std::unordered_map<Resource* , String>[typeCount];
	}

	Result ResourceManager::AddResource(StringView name, Resource* resource, size_t typeIndex)
	{
		if (!registry.IsValidTypeIndex(typeIndex))
			return BLAZE_ERROR_RESULT("Blaze Engine", "Invalid type index");

		resources[typeIndex].emplace_back(resource);
		resourceMap[typeIndex].insert({ name, resource });
		resourceToNameMap[typeIndex].insert({ resource, name });

		return Result();
	}

	Result ResourceManager::AddResource(Resource* resource, size_t typeIndex)
	{
		if (!registry.IsValidTypeIndex(typeIndex))
			return BLAZE_ERROR_RESULT("Blaze Engine", "Invalid type index");

		resources[typeIndex].emplace_back(resource);

		return Result();
	}

	Result ResourceManager::RemoveResource(Resource* resource, size_t typeIndex)
	{
		{
			auto& map = resourceMap[typeIndex];
			auto it = map.find(resourceToNameMap[typeIndex][resource]);
			if (it != map.end())
				map.erase(it);			
		}
		{
			auto& map = resourceToNameMap[typeIndex];
			auto it = map.find(resource);
			if (it != map.end())
				map.erase(it);
		}
		{
			auto& list = resources[typeIndex];
			auto it = list.begin();
			for (; it != list.end(); ++it)
				if (*it == resource)
					break;

			if (it != list.end())
				list.erase(it);
		}

		return Result();
	}

	Result ResourceManager::RemoveResource(StringView name, size_t typeIndex)
	{
		RemoveResource(resourceMap[typeIndex][name], typeIndex);
		return Result();
	}

	Resource* ResourceManager::GetResource(StringView name, size_t typeIndex)
	{		
		auto& map = resourceMap[typeIndex];
		auto it = map.find(name);
		if (it == map.end())
			return nullptr;
		return
			it->second;
	}

	Resource* ResourceManager::GetResource(uint index, size_t typeIndex)
	{
		return resources[typeIndex][index];
	}

	String ResourceManager::GetResourceName(Resource* resource, uint typeIndex)
	{
		auto& map = resourceToNameMap[typeIndex];
		auto it = map.find(resource);
		if (it != map.end())
			return it->second;
		else
			return "";
	}

	bool ResourceManager::HasResource(StringView name, size_t typeIndex)
	{
		auto it = resourceMap[typeIndex].find(name);
		return it != resourceMap[typeIndex].end();
	}

	uint ResourceManager::GetResourceCount(size_t typeIndex)
	{ 
		return resources[typeIndex].size();
	}

}