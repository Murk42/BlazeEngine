#pragma once
#include "BlazeEngine/Application/ResourceSystem/ResourceTypeRegistry.h"
#include "BlazeEngine/Application/ResourceSystem/Resource.h"

namespace Blaze::ResourceSystem
{

	class BLAZE_API ResourceManager
	{
		ResourceTypeRegistry registry;
		uint typeCount;

		std::vector<Resource*>* resources;
		std::unordered_map<String, Resource*>* resourceMap;
		std::unordered_map<Resource*, String>* resourceToNameMap;
	public:
		ResourceManager();
		~ResourceManager();

		void SetResourceTypeRegistry(const ResourceTypeRegistry&);

		template<typename T>
		void AddResource(StringView name, T* resource)
		{
			AddResource(name, resource, registry.GetResourceTypeIndex<T>());
		}
		template<typename T>
		void AddResource(T* resource)
		{
			//This line gives an error for no reason
			//AddResource(resource, registry.GetTypeIndex<typename T>());
		}
		void AddResource(Resource* resource, size_t typeIndex);
		void AddResource(StringView name, Resource* resource, size_t typeIndex);

		template<typename T>
		void RemoveResource(T* resource)
		{
			RemoveResource(resource, registry.GetResourceTypeIndex<T>());
		}
		template<typename T>
		void RemoveResource(StringView name)
		{
			RemoveResource(name, registry.GetResourceTypeIndex<T>());
		}
		Result RemoveResource(Resource* resource, size_t typeIndex);
		Result RemoveResource(StringView name, size_t typeIndex);

		template<typename T>
		T* GetResource(StringView name)
		{
			return (T*)GetResource(name, registry.GetResourceTypeIndex<T>());
		}
		template<typename T>
		T* GetResource(uint index)
		{
			return (T*)GetResource(index, registry.GetResourceTypeIndex<T>());
		}
		Resource* GetResource(StringView name, size_t typeIndex);
		Resource* GetResource(uint index, size_t typeIndex);

		String GetResourceName(Resource* resource, uint typeIndex);

		template<typename T>
		bool HasResource(StringView name)
		{
			return HasResource(name, registry.GetResourceTypeIndex<T>());
		}
		bool HasResource(StringView name, size_t typeIndex);		

		template<typename T>
		uint GetResourceCount()
		{			
			return GetResourceCount(registry.GetResourceTypeIndex<T>());
		}
		uint GetResourceCount(size_t typeIndex);

		const ResourceTypeRegistry& GetResourceTypeRegistry() const { return registry; }
	};
}