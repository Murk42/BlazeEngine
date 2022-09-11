#pragma once
#include "BlazeEngine/Application/ResourceSystem/ResourceTypeRegistry.h"

namespace Blaze::Resource
{
	class BLAZE_API ResourceManager
	{
		ResourceTypeRegistry registry;
		uint typeCount;

		std::vector<void*>* resources;
		std::unordered_map<String, void*>* resourceMap;
	public:
		ResourceManager();
		~ResourceManager();

		void SetResourceTypeRegistry(const ResourceTypeRegistry&);

		template<typename T>
		void AddResource(StringView name, T* resource)
		{
			AddResource(name, resource, registry.GetTypeIndex<T>());
		}
		void AddResource(StringView name, void* resource, size_t typeIndex);		

		template<typename T>
		void AddResource(T* resource)
		{
			//This line gives an error for no reason
			//AddResource(resource, registry.GetTypeIndex<typename T>());
		}
		void AddResource(void* resource, size_t typeIndex);

		template<typename T>
		T* GetResource(StringView name)
		{
			return (T*)GetResource(name, registry.GetTypeIndex<T>());
		}
		void* GetResource(StringView name, size_t typeIndex);

		template<typename T>
		T* GetResource(uint index)
		{
			return (T*)GetResource(index, registry.GetTypeIndex<T>());
		}
		void* GetResource(uint index, size_t typeIndex);

		template<typename T>
		uint GetResourceCount()
		{			
			return GetResourceCount(registry.GetTypeIndex<T>());			
		}
		uint GetResourceCount(size_t typeIndex);


		const ResourceTypeRegistry& GetResourceTypeRegistry() const { return registry; }
	};
}