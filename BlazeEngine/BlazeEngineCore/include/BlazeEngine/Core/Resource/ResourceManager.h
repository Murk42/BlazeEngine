#pragma once
#include "BlazeEngine/Core/Resource/DefaultResourceStorage.h"
#include "BlazeEngine/Core/Container/VirtualMap.h"
#include "BlazeEngine/Core/Container/List.h"
#include "BlazeEngine/Core/Threading/ThreadPool.h"

namespace Blaze
{
	class BLAZE_API ResourceManager
	{
	public:
		template<typename T>
		using LoadFunction = void(*)(ResourceRef<T> resource, void*);
		template<typename T>
		using ResourceLoadedCallback = void(*)(ResourceRef<T> resource, void*);

		ResourceManager();

		template<typename T, std::invocable<ResourceRef<T>> Callable>
		ResourceRef<T> LoadResourceAsync(StringView name, Callable loadFunction);
		template<typename T>
		ResourceRef<T> LoadResourceAsync(StringView name, LoadFunction<T> loadFunction, void* userData);

		template<typename T, std::invocable<ResourceRef<T>> Callable, std::invocable<ResourceRef<T>> ResourceLoadedCallable>
		ResourceRef<T> LoadResourceAsync(StringView name, Callable loadFunction, ResourceLoadedCallable resourceLoadedCallback);
		template<typename T>
		ResourceRef<T> LoadResourceAsync(StringView name, LoadFunction<T> loadFunction, void* userData, ResourceLoadedCallback<T> resourceLoadedCallback, void* resourceLoadedCallbackUserData);

		template<typename T, typename ... Args> requires IsConstructibleFrom<T, Args...>
		ResourceRef<T> LoadResource(StringView name, Args&& ... args);

		template<typename T>
		ResourceRef<T> GetResource(StringView name);

		//template<typename T>
		//void UnloadResource(StringView name);
		//template<typename T>
		//void UnloadResource(Resource<T>& resourceBase);

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
		ResourceRef<T> AllocateResource(StringView name);
	};

	template<typename T, std::invocable<ResourceRef<T>> Callable>
	inline ResourceRef<T> ResourceManager::LoadResourceAsync(StringView name, Callable loadFunction)
	{
		ResourceRef<T> resource = AllocateResource<T>(name);

		struct ThreadData
		{
			ResourceRef<T> resource;
			Callable loadFunction;
		};

		ThreadData* threadData = new ThreadData{
			resource,
			std::move(loadFunction)
		};

		threadPool.RunTask([](void* userData) {
			ThreadData threadData = std::move(*(ThreadData*)userData);
			delete (ThreadData*)userData;

			threadData.loadFunction(std::move(threadData.resource));

			}, threadData);

		return resource;
	}
	template<typename T>
	inline ResourceRef<T> ResourceManager::LoadResourceAsync(StringView name, LoadFunction<T> loadFunction, void* loadFunctionUserData)
	{
		ResourceRef<T> resource = AllocateResource<T>(name);

		struct ThreadData
		{
			ResourceRef<T> resource;
			LoadFunction<T> loadFunction;
			void* loadFunctionUserData;
		};

		ThreadData* threadData = new ThreadData{
			resource,
			loadFunction,
			loadFunctionUserData,
		};

		threadPool.RunTask([](void* userData) {
			ThreadData threadData = *(ThreadData*)userData;
			delete (ThreadData*)userData;

			threadData.loadFunction(std::move(threadData.resource), threadData.loadFunctionUserData);

			}, threadData);

		return resource;
	}
	template<typename T, std::invocable<ResourceRef<T>> Callable, std::invocable<ResourceRef<T>> ResourceLoadedCallable>
	ResourceRef<T> ResourceManager::LoadResourceAsync(StringView name, Callable loadFunction, ResourceLoadedCallable resourceLoadedCallback)
	{
		ResourceRef<T> resource = AllocateResource<T>(name);

		struct ThreadData
		{
			ResourceManager& resourceManager;
			ResourceRef<T> resource;

			Callable loadFunction;
			ResourceLoadedCallable* resourceLoadedCallback;
		};

		ThreadData* threadData = new ThreadData{
			*this,
			resource,
			std::move(loadFunction),
			new ResourceLoadedCallable(std::move(resourceLoadedCallback))
		};

		threadPool.RunTask([](void* userData) {
			ThreadData threadData = std::move(*(ThreadData*)userData);
			delete (ThreadData*)userData;

			threadData.loadFunction(threadData.resource);

			auto CallCallbackFunction = [](void* resource, void* callbackPtr, void* userData) {
				ResourceLoadedCallable callback = std::move(*(ResourceLoadedCallable*)callbackPtr);
				delete (ResourceLoadedCallable*)callbackPtr;

				callback(*(ResourceRef<T>*)resource);
				};

			ResourceLoadedCallbackData resourceLoadedCallbackData{
				.function = CallCallbackFunction,
				.resourceLoadedCallback = threadData.resourceLoadedCallback,
				.resourceLoadedCallbackUserData = nullptr,
				.resource = &threadData.resource
			};

			std::lock_guard lg{ threadData.resourceManager.mutex };

			threadData.resourceManager.resourceLoadedCallbacks.AddBack(resourceLoadedCallbackData);

			}, threadData);

		return resource;
	}
	template<typename T>
	inline ResourceRef<T> ResourceManager::LoadResourceAsync(StringView name, LoadFunction<T> loadFunction, void* loadFunctionUserData, ResourceLoadedCallback<T> resourceLoadedCallback, void* resourceLoadedCallbackUserData)
	{
		ResourceRef<T> resource = AllocateResource<T>(name);

		struct ThreadData
		{
			ResourceManager& resourceManager;
			ResourceRef<T> resource;

			LoadFunction<T> loadFunction;
			void* loadFunctionUserData;

			ResourceLoadedCallback<T> resourceLoadedCallback;
			void* resourceLoadedCallbackUserData;
		};

		ThreadData* threadData = new ThreadData{
			*this,
			resource,
			loadFunction,
			loadFunctionUserData,
			resourceLoadedCallback,
			resourceLoadedCallbackUserData
		};

		threadPool.RunTask([](void* userData) {
			ThreadData threadData = *(ThreadData*)userData;
			delete (ThreadData*)userData;

			threadData.loadFunction(threadData.resource, threadData.userData);

			auto CallCallbackFunction = [](void* resource, void* callback, void* userData) {
				((ResourceLoadedCallback<T>)callback)(*(ResourceRef<T>*)resource, userData);
				};

			ResourceLoadedCallbackData resourceLoadedCallbackData{
				.function = CallCallbackFunction,
				.resourceLoadedCallback = threadData.resourceLoadedCallback,
				.resourceLoadedCallbackUserData = threadData.resourceLoadedCallbackUserData,
				.resource = &threadData.resource
			};

			std::lock_guard lg{ threadData.resourceManager.mutex };

			threadData.resourceManager.resourceLoadedCallbacks.AddBack(resourceLoadedCallbackData);
			}, threadData);

		return resource;
	}
	template<typename T, typename ...Args> requires IsConstructibleFrom<T, Args...>
	inline ResourceRef<T> ResourceManager::LoadResource(StringView name, Args && ...args)
	{
		ResourceRef<T> resource = AllocateResource<T>(name);
		resource.GetResource()->LoadResource(std::forward<Args>(args)...);
		return resource;
	}
	template<typename T>
	inline ResourceRef<T> ResourceManager::GetResource(StringView name)
	{
		uint64 typeID = Resource<T>::GetTypeId();

		auto it = resourceStorages.Find(typeID);

		if (it.IsNull())
			return ResourceRef<T>();

		ResourceStorage<T>* resourceStorage = it.GetValue<ResourceStorage<T>>();

		return resourceStorage->GetResource(name);
	}
	template<typename T>
	inline ResourceRef<T> ResourceManager::AllocateResource(StringView name)
	{
		uint64 typeID = Resource<T>::GetTypeId();

		auto it = resourceStorages.Find(typeID);

		ResourceStorage<T>* resourceStorage = nullptr;

		if (it.IsNull())
		{
			resourceStorage = resourceStorages.Insert<DefaultResourceStorage<T>>(typeID).value;
		}
		else
			resourceStorage = it.GetValue<ResourceStorage<T>>();

		return resourceStorage->CreateResource(name);
	}

}