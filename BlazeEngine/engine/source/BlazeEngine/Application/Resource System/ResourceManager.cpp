#include "BlazeEngine/Application/ResourceSystem/ResourceManager.h"

namespace Blaze::Resource
{
	ResourceManager::ResourceManager()
		: resources(nullptr), resourceMap(nullptr), typeCount(0)
	{
		SetResourceTypeRegistry(ResourceTypeRegistry::CoreRegistry());
	}

	ResourceManager::~ResourceManager()
	{
		for (size_t i = 0; i < typeCount; ++i)
			resourceMap[i].clear();
		delete[] resources;
		delete[] resourceMap;
	}

	void ResourceManager::SetResourceTypeRegistry(const ResourceTypeRegistry& registry)
	{
		for (size_t i = 0; i < typeCount; ++i)
			resourceMap[i].clear();
		delete[] resources;
		delete[] resourceMap;

		this->registry = registry;
		typeCount = registry.GetResourceTypeCount();

		resources = new std::vector<void*>[typeCount];
		resourceMap = new std::unordered_map<String, void*>[typeCount];
	}

	void ResourceManager::AddResource(StringView name, void* resource, size_t typeIndex)
	{
		resources[typeIndex].emplace_back(resource);
		resourceMap[typeIndex].insert({ name, resource });
	}

	void ResourceManager::AddResource(void* resource, size_t typeIndex)
	{
		resources[typeIndex].emplace_back(resource);
	}

	void* ResourceManager::GetResource(StringView name, size_t typeIndex)
	{		
		return resourceMap[typeIndex][name];
	}

	void* ResourceManager::GetResource(uint index, size_t typeIndex)
	{
		return resources[typeIndex][index];
	}

	uint ResourceManager::GetResourceCount(size_t typeIndex)
	{ 
		return resources[typeIndex].size();
	}

}