#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Application/ResourceSystem/ResourceTypeRegistry.h"

namespace Blaze::Resource
{
	class ResourceManager;

	class BLAZE_API ResourceStorage
	{
		ResourceTypeRegistry registry;
		size_t typeCount;
		
		ResourceManager* manager;

		std::list<void*>* resources;
	public:
		ResourceStorage();
		~ResourceStorage();

		void SetResourceTypeRegistry(const ResourceTypeRegistry&);
		void SetResourceManager(ResourceManager* manager);

		template<typename T>
		T* CreateResource()
		{
			return (T*)CreateResource(registry.GetTypeIndex<T>());
		}
		void* CreateResource(size_t typeIndex);		

		inline const ResourceTypeRegistry& GetResourceTypeRegistry() const { return registry; }
		inline ResourceManager* GetResourceManager() const { return manager; }
	};
}