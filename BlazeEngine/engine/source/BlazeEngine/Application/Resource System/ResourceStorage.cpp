#include "BlazeEngine/Application/ResourceSystem/ResourceStorage.h"
#include "BlazeEngine/Application/ResourceSystem/ResourceManager.h"
#include "BlazeEngine/Core/MemoryManager.h"

namespace Blaze::Resource
{
	ResourceStorage::ResourceStorage()
		: resources(nullptr), typeCount(0), manager(nullptr)
	{
		SetResourceTypeRegistry(ResourceTypeRegistry::CoreRegistry());
	}

	ResourceStorage::~ResourceStorage()
	{
		for (int i = 0; i < typeCount; ++i)
		{
			ResourceTypeData typeData = registry.GetResourceTypeData(i);

			for (auto& res : resources[i])
			{
				typeData.destruct(res);
				Memory::Free(res);
			}
		}

		delete[] resources;
	}

	void ResourceStorage::SetResourceTypeRegistry(const ResourceTypeRegistry& typeRegistry)
	{

		for (int i = 0; i < typeCount; ++i)
		{
			ResourceTypeData typeData = registry.GetResourceTypeData(i);

			for (auto& res : resources[i])
			{
				typeData.destruct(res);
				Memory::Free(res);
			}
		}

		delete[] resources;

		registry = typeRegistry;
		typeCount = registry.GetResourceTypeCount();

		resources = new std::list<void*>[typeCount];
	}

	void ResourceStorage::SetResourceManager(ResourceManager* manager)
	{
		SetResourceTypeRegistry(manager->GetResourceTypeRegistry());
		this->manager = manager;
	}

	void* ResourceStorage::CreateResource(size_t typeIndex)
	{
		ResourceTypeData typeData = registry.GetResourceTypeData(typeIndex);

		void* ptr = Memory::Allocate(typeData.size);

		typeData.construct(ptr);

		if (manager != nullptr)
			manager->AddResource(ptr, typeIndex);

		resources[typeIndex].emplace_back(ptr);
		return ptr;
	}

	void* ResourceStorage::CreateResource(StringView name, size_t typeIndex)
	{
		ResourceTypeData typeData = registry.GetResourceTypeData(typeIndex);

		void* ptr = Memory::Allocate(typeData.size);

		typeData.construct(ptr);

		if (manager != nullptr)
			manager->AddResource(name, ptr, typeIndex);

		resources[typeIndex].emplace_back(ptr);
		return ptr;
	}

}