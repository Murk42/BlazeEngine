#pragma once
#include "BlazeEngine/Application/ResourceSystem/ResourceTypeRegistry.h"
#include "BlazeEngine/Application/ResourceSystem/Resource.h"

namespace Blaze::ResourceSystem
{

	class BLAZE_API ResourceManager
	{
		ResourceTypeRegistry registry;
		uintMem typeCount;

		Array<Resource*>* resources;
		Map<String, Resource*>* resourceMap;
		Map<Resource*, String>* resourceToNameMap;
	public:
		ResourceManager();
		~ResourceManager();

		void SetResourceTypeRegistry(const ResourceTypeRegistry&);

		template<typename T>
		Result AddResource(StringView name, T* resource)
		{
			return AddResource(name, resource, registry.GetResourceTypeIndex<T>());
		}
		template<typename T>
		void AddResource(T* resource)
		{
			//This line gives an error for no reason
			//AddResource(resource, registry.GetTypeIndex<typename T>());
		}
		Result AddResource(Resource* resource, uintMem typeIndex);
		Result AddResource(StringView name, Resource* resource, uintMem typeIndex);

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
		Result RemoveResource(Resource* resource, uintMem typeIndex);
		Result RemoveResource(StringView name, uintMem typeIndex);

		template<typename T>
		T* GetResource(StringView name)
		{
			return (T*)GetResource(name, registry.GetResourceTypeIndex<T>());
		}
		template<typename T>
		T* GetResource(uintMem index)
		{
			return (T*)GetResource(index, registry.GetResourceTypeIndex<T>());
		}
		Resource* GetResource(StringView name, uintMem typeIndex);
		Resource* GetResource(uintMem index, uintMem typeIndex);

		String GetResourceName(Resource* resource, uintMem typeIndex);

		template<typename T>
		bool HasResource(StringView name)
		{
			return HasResource(name, registry.GetResourceTypeIndex<T>());
		}
		bool HasResource(StringView name, uintMem typeIndex);

		template<typename T>
		uintMem GetResourceCount()
		{			
			return GetResourceCount(registry.GetResourceTypeIndex<T>());
		}
		uintMem GetResourceCount(uintMem typeIndex);

		const ResourceTypeRegistry& GetResourceTypeRegistry() const { return registry; }
	};
}