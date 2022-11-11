#include "BlazeEngine/Application/ResourceSystem/ResourceStorage.h"
#include "BlazeEngine/Application/ResourceSystem/ResourceManager.h"
#include "BlazeEngine/Memory/MemoryManager.h"

namespace Blaze::ResourceSystem
{
	ResourceStorage::ResourceStorage()
		: resources(nullptr), manager(nullptr)
	{		
	}

	ResourceStorage::~ResourceStorage()
	{
		if (manager == nullptr)
			return;

		const auto& registry = manager->GetResourceTypeRegistry();
		size_t typeCount = registry.GetResourceTypeCount();

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

	void ResourceStorage::SetResourceManager(ResourceManager* manager)
	{
		if (this->manager != nullptr)
		{
			const auto& registry = this->manager->GetResourceTypeRegistry();
			size_t typeCount = registry.GetResourceTypeCount();

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

		this->manager = manager;

		const auto& registry = this->manager->GetResourceTypeRegistry();
		size_t typeCount = registry.GetResourceTypeCount();

		resources = new std::list<Resource*>[typeCount];
		
	}

	Resource* ResourceStorage::CreateResource(size_t typeIndex)
	{
		const auto& registry = manager->GetResourceTypeRegistry();		

		ResourceTypeData typeData = registry.GetResourceTypeData(typeIndex);

		Resource* ptr = (Resource*)Memory::Allocate(typeData.size);

		typeData.construct(ptr);
		
		manager->AddResource(ptr, typeIndex);

		resources[typeIndex].emplace_back(ptr);
		return ptr;
	}

	Resource* ResourceStorage::CreateResource(StringView name, size_t typeIndex)
	{
		const auto& registry = manager->GetResourceTypeRegistry();		

		ResourceTypeData typeData = registry.GetResourceTypeData(typeIndex);

		Resource* ptr = (Resource*)Memory::Allocate(typeData.size);

		typeData.construct(ptr);
		ptr->name = name;
		manager->AddResource(name, ptr, typeIndex);

		resources[typeIndex].emplace_back(ptr);
		return ptr;
	}

	Result ResourceStorage::DestroyResource(Resource* resource, size_t typeIndex)
	{		
		const auto& registry = manager->GetResourceTypeRegistry();
		ResourceTypeData typeData = registry.GetResourceTypeData(typeIndex);

		auto& list = resources[typeIndex];
		auto it = list.begin();

		for (; it != list.end(); ++it)
			if (*it == resource)
				break;

		list.erase(it);

		manager->RemoveResource(resource, typeIndex);

		typeData.destruct(resource);
		Memory::Free(resource);

		return Result();
	}

	Result ResourceStorage::DestroyResource(StringView name, size_t typeIndex)
	{
		return DestroyResource(manager->GetResource(name, typeIndex), typeIndex);
	}

	const std::list<Resource*>& ResourceStorage::GetResourceList(size_t typeIndex) const
	{
		return resources[typeIndex];
	}
}