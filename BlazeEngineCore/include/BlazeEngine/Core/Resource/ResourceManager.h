#pragma once
#include "BlazeEngine/Core/Resource/DefaultResourceStorage.h"
#include "BlazeEngine/Core/Container/VirtualMap.h"
#include "BlazeEngine/Core/Threading/ThreadPool.h"

namespace Blaze
{
	class BLAZE_API ResourceManager
	{
	public:
		ResourceManager();

		template<typename T, typename ... Args> requires IsConstructibleFrom<T, Args...>
		ResourceRef<T> LoadNamedResource(StringView name, Args&& ... args);
		template<typename T, typename ... Args> requires IsConstructibleFrom<T, Args...>
		ResourceRef<T> LoadResource(Args&& ... args);

		template<typename T>
		ResourceRef<T> GetNamedResource(StringView name);

		void HandleResourceLoadedCallbacks();
	private:
		VirtualMap<uint64> resourceStorages;
		ThreadPool threadPool;

		struct ResourceLoadedCallbackData
		{
			//First argument is pointer to Resource<T>, second ResourceLoadedCallback<T> and this void* userData
			void(*function)(void*, void*, void*);
			void* resourceLoadedCallback;
			void* resourceLoadedCallbackUserData;
			void* resource;
		};

		std::mutex mutex;
		Array<ResourceLoadedCallbackData> resourceLoadedCallbacks;

		template<typename T>
		ResourceStorage<T>& GetResourceStorage();
	};

	template<typename T, typename ...Args> requires IsConstructibleFrom<T, Args...>
	inline ResourceRef<T> ResourceManager::LoadNamedResource(StringView name, Args && ...args)
	{
		ResourceStorage<T>& resourceStorage = GetResourceStorage<T>();
		ResourceRef<T> resource = resourceStorage.AllocateNamedResource(name);
		resource.GetResource()->Construct(std::forward<Args>(args)...);
		return resource;
	}
	template<typename T, typename ...Args> requires IsConstructibleFrom<T, Args...>
	inline ResourceRef<T> ResourceManager::LoadResource(Args && ...args)
	{
		ResourceStorage<T>& resourceStorage = GetResourceStorage<T>();
		ResourceRef<T> resource = resourceStorage.AllocateResource();
		resource.GetResource()->Construct(std::forward<Args>(args)...);
		return resource;
	}
	template<typename T>
	inline ResourceRef<T> ResourceManager::GetNamedResource(StringView name)
	{
		uint64 typeID = Resource<T>::GetTypeId();

		auto it = resourceStorages.Find(typeID);

		if (it.IsNull())
			return ResourceRef<T>();

		ResourceStorage<T>* resourceStorage = it.GetValue<ResourceStorage<T>>();

		return resourceStorage->GetNamedResource(name);
	}
	template<typename T>
	inline ResourceStorage<T>& ResourceManager::GetResourceStorage()
	{
		uint64 typeID = Resource<T>::GetTypeId();

		auto it = resourceStorages.Find(typeID);

		ResourceStorage<T>* resourceStorage = nullptr;

		if (it.IsNull())
			return *resourceStorages.Insert<DefaultResourceStorage<T>>(typeID).value;
		else
			return *it.GetValue<ResourceStorage<T>>();
	}
}