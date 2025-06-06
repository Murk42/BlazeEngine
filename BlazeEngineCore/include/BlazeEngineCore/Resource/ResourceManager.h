#pragma once
#include "BlazeEngineCore/Resource/DefaultResourceStorage.h"
#include "BlazeEngineCore/DataStructures/VirtualMap.h"
#include "BlazeEngineCore/DataStructures/List.h"
#include "BlazeEngineCore/Threading/ThreadPool.h"

namespace Blaze
{
	class BLAZE_CORE_API ResourceManager
	{
	public:
		template<typename T>
		using LoadFunction = void(*)(Resource<T>& resource, void*);
		template<typename T>
		using ResourceLoadedCallback = void(*)(Resource<T>& resource, void*);

		ResourceManager();

		template<typename T, std::invocable<Resource<T>&> Callable>
		Resource<T>& LoadResourceAsync(StringView name, Callable loadFunction);
		template<typename T>
		Resource<T>& LoadResourceAsync(StringView name, LoadFunction<T> loadFunction, void* userData);

		template<typename T, std::invocable<Resource<T>&> Callable, std::invocable<Resource<T>&> ResourceLoadedCallable>
		Resource<T>& LoadResourceAsync(StringView name, Callable loadFunction, ResourceLoadedCallable resourceLoadedCallback);
		template<typename T>
		Resource<T>& LoadResourceAsync(StringView name, LoadFunction<T> loadFunction, void* userData, ResourceLoadedCallback<T> resourceLoadedCallback, void* resourceLoadedCallbackUserData);

		template<typename T>
		Resource<T>* GetResource(StringView name);

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
		Resource<T>& AllocateResource(StringView name);
	};

	template<typename T, std::invocable<Resource<T>&> Callable>
	inline Resource<T>& ResourceManager::LoadResourceAsync(StringView name, Callable loadFunction)
	{	
		Resource<T>& resource = AllocateResource<T>(name);

		struct ThreadData
		{
			Resource<T>& resource;
			Callable loadFunction;
		};

		ThreadData* threadData = new ThreadData{
			resource,
			std::move(loadFunction)
		};

		threadPool.RunTask([](void* userData) {
			ThreadData threadData = std::move(*(ThreadData*)userData);
			delete (ThreadData*)userData;

			threadData.loadFunction(threadData.resource);

			}, threadData);

		return resource;
	}
	template<typename T>
	inline Resource<T>& ResourceManager::LoadResourceAsync(StringView name, LoadFunction<T> loadFunction, void* loadFunctionUserData)
	{		
		Resource<T>& resource = AllocateResource<T>(name);

		struct ThreadData
		{			
			Resource<T>& resource;
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

			threadData.loadFunction(threadData.resource, threadData.loadFunctionUserData);

			}, threadData);

		return resource;
	}
	template<typename T, std::invocable<Resource<T>&> Callable, std::invocable<Resource<T>&> ResourceLoadedCallable>
	Resource<T>& ResourceManager::LoadResourceAsync(StringView name, Callable loadFunction, ResourceLoadedCallable resourceLoadedCallback)
	{
		Resource<T>& resource = AllocateResource<T>(name);

		struct ThreadData
		{
			ResourceManager& resourceManager;
			Resource<T>& resource;

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

				callback(*(Resource<T>*)resource);			
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
	inline Resource<T>& ResourceManager::LoadResourceAsync(StringView name, LoadFunction<T> loadFunction, void* loadFunctionUserData, ResourceLoadedCallback<T> resourceLoadedCallback, void* resourceLoadedCallbackUserData)
	{
		Resource<T>& resource = AllocateResource<T>(name);

		struct ThreadData
		{
			ResourceManager& resourceManager;
			Resource<T>& resource;

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
				((ResourceLoadedCallback<T>)callback)(*(Resource<T>*)resource, userData);
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
	template<typename T>
	inline Resource<T>* ResourceManager::GetResource(StringView name)
	{
		uint64 typeID = Resource<T>::GetTypeId();

		auto it = resourceStorages.Find(typeID);

		if (it.IsNull())		
			return nullptr;					

		ResourceStorage<T>* resourceStorage = it.GetValue<ResourceStorage<T>>();
		
		return resourceStorage->GetResource(name);
	}	
	template<typename T>
	inline Resource<T>& ResourceManager::AllocateResource(StringView name)
	{
		uint64 typeID = Resource<T>::GetTypeId();

		auto it = resourceStorages.Find(typeID);

		ResourceStorage<T>* resourceStorage = nullptr; 

		if (it.IsNull())
		{
			auto [newIt, inserted, _resourceStorage] = resourceStorages.Insert<DefaultResourceStorage<T>>(typeID);

			resourceStorage = _resourceStorage;
		}
		else
			resourceStorage = it.GetValue<ResourceStorage<T>>();

		auto& resource = resourceStorage->CreateResource(name);

		resource = Resource<T>();

		return resource;
	}

}