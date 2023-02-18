#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Core/ResultValue.h"
#include "BlazeEngine/Application/ResourceSystem/ResourceTypeRegistry.h"
#include "BlazeEngine/Application/ResourceSystem/Resource.h"
#include "BlazeEngine/Application/ResourceSystem/ResourceManager.h"

namespace Blaze::ResourceSystem
{
	class ResourceManager;

	class BLAZE_API ResourceStorage
	{
		ResourceManager* manager;

		std::list<Resource*>* resources;
	public:
		ResourceStorage();
		~ResourceStorage();
		
		void SetResourceManager(ResourceManager* manager);

		template<typename T>
		ResultValue<T*> CreateResource()
		{			
			return (T*)CreateResource(manager->GetResourceTypeRegistry().GetResourceTypeIndex<T>());
		}
		template<typename T>
		ResultValue<T*> CreateResource(StringView name)
		{			
			return (T*)CreateResource(name, manager->GetResourceTypeRegistry().GetResourceTypeIndex<T>());
		}
		Resource* CreateResource(size_t typeIndex);
		Resource* CreateResource(StringView name, size_t typeIndex);

		template<typename T>
		Result DestroyResource(T* resource)
		{
			return DestroyResource(resource, manager->GetResourceTypeRegistry().GetResourceTypeIndex<T>());
		}
		template<typename T>
		Result DestroyResource(StringView name)
		{
			return DestroyResource(name, manager->GetResourceTypeRegistry().GetResourceTypeIndex<T>());
		}
		Result DestroyResource(Resource* resource, size_t typeIndex);
		Result DestroyResource(StringView name, size_t typeIndex);
		
		template<typename T>
		const std::list<Resource*>& GetResourceList() const
		{
			return GetResourceList(manager->GetResourceTypeRegistry().GetResourceTypeIndex<T>());
		}
		const std::list<Resource*>& GetResourceList(size_t typeIndex) const;
		
		inline ResourceManager* GetResourceManager() const { return manager; }
	};
}