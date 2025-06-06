#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"
#include "BlazeEngineCore/DataStructures/Hash.h"
#include <atomic>

namespace Blaze
{
	template<typename T>
	class BLAZE_CORE_API Resource
	{
	public:
		template<typename ... Args>
		Resource(Args&& ... args) requires std::constructible_from<T, Args...>;
		Resource(T&& resource) requires std::move_constructible<T>;
		Resource(Resource&& other) requires std::move_constructible<T>;

		bool IsLoaded() const;

		T& WaitToLoad();
		T* Get();
		const T* Get() const;

		Resource<T>& operator=(const T&) = delete;
		Resource<T>& operator=(T&& resource) noexcept;
		Resource<T>& operator=(const Resource<T>&) = delete;
		Resource<T>& operator=(Resource<T>&& other) noexcept;

		static uint64 GetTypeId()
		{
			volatile auto file = __FILE__;
			volatile auto line = __LINE__;			
			return (uint64)&GetTypeId;
		}
	private:
		std::atomic_flag loaded;
		T resource;
	};	
	template<typename T>
	template<typename ...Args>
	inline Resource<T>::Resource(Args&& ...args) requires std::constructible_from<T, Args...>
		: resource(std::forward<Args>(args)...)
	{
		loaded.test_and_set();
	}
	template<typename T>
	inline Resource<T>::Resource(T&& resource) requires std::move_constructible<T>
		: resource(std::move(resource)) 
	{ 
		loaded.test_and_set(); 
	}
	template<typename T>
	inline Resource<T>::Resource(Resource&& other) requires std::move_constructible<T>
		: resource(std::move(resource)) 
	{ 
		if (other.loaded.test())
		{
			loaded.test_and_set();
			other.loaded.clear();
		}
		else
			loaded.clear();
	}
	template<typename T>
	inline bool Resource<T>::IsLoaded() const
	{
		return loaded.test();
	}
	template<typename T>
	inline T& Resource<T>::WaitToLoad()
	{		
		loaded.wait(false);
		return resource;
	}
	template<typename T>
	inline T* Resource<T>::Get()
	{
		if (loaded.test())		
			return &resource;

		return nullptr;
	}
	template<typename T>
	inline const T* Resource<T>::Get() const
	{
		if (loaded.test())
			return &resource;

		return nullptr;
	}
	template<typename T>
	inline Resource<T>& Resource<T>::operator=(T&& resource) noexcept
	{
		this->resource = std::move(resource);

		loaded.test_and_set();
		loaded.notify_all();

		return *this;
	}
	template<typename T>
	inline Resource<T>& Resource<T>::operator=(Resource<T>&& other) noexcept
	{
		resource = std::move(other.resource);

		if (other.loaded.test())
		{
			loaded.test_and_set();
			loaded.notify_all();
			other.loaded.clear();
		}
		return *this;
	}

}