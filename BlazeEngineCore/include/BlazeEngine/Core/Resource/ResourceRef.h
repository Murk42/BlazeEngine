#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/String/StringView.h"
#include "BlazeEngine/Core/Resource/Resource.h"
#include "BlazeEngine/Core/Resource/ResourceManager.h"

namespace Blaze
{
	class ResourceBase;

	class BLAZE_API ResourceBaseRef
	{
	public:
		ResourceBaseRef();
		ResourceBaseRef(ResourceBase& resource);
		ResourceBaseRef(const ResourceBaseRef& other);
		ResourceBaseRef(ResourceBaseRef&& other) noexcept;
		~ResourceBaseRef();

		bool IsNull() const;
		bool IsLoaded() const;
		void WaitToLoad() const;

		uint32 Hash() const;

		ResourceBase* GetResource() const;
		template<typename T>
		T* GetValue() const;

		operator bool() const;

		bool operator==(const ResourceBaseRef& other) const;
		bool operator!=(const ResourceBaseRef& other) const;

		ResourceBaseRef& operator=(const ResourceBaseRef& other);
		ResourceBaseRef& operator=(ResourceBaseRef&& other) noexcept;
	private:
		ResourceBase* resourceBase;
	};

	template<typename T>
	class BLAZE_API ResourceRef : public ResourceBaseRef
	{
	public:
		using Type = T;

		ResourceRef();
		ResourceRef(const ResourceRef& other) = default;
		ResourceRef(ResourceRef&& other) noexcept = default;
		ResourceRef(Resource<T>& resource);
		template<typename ... Args> requires IsConstructibleFrom<T, Args...>
		ResourceRef(ResourceManager& resourceManager, StringView name, Args&& ... args);
		~ResourceRef() = default;

		Resource<T>* GetResource() const;
		T* GetValue() const;

		T& operator*();
		const T& operator*() const;
		T* operator->();
		const T* operator->() const;

		bool operator==(const ResourceRef& other) const = default;
		bool operator!=(const ResourceRef& other) const = default;

		ResourceRef& operator=(const ResourceRef& other) = default;
		ResourceRef& operator=(ResourceRef&& other) noexcept = default;
	};

	template<typename T>
	inline T* ResourceBaseRef::GetValue() const
	{
		Resource<T>* resource = dynamic_cast<Resource<T>*>(ResourceBaseRef::GetResource());

		if (resource == nullptr)
			return nullptr;

		return &resource->Get();
	}

	template<typename T>
	inline ResourceRef<T>::ResourceRef()
	{
	}
	template<typename T>
	inline ResourceRef<T>::ResourceRef(Resource<T>& resource)
		: ResourceBaseRef(resource)
	{
	}
	template<typename T>
	template<typename ...Args>  requires IsConstructibleFrom<T, Args...>
	inline ResourceRef<T>::ResourceRef(ResourceManager& resourceManager, StringView name, Args && ...args)
		: ResourceRef(resourceManager.LoadResource<T>(name, std::forward<Args>(args)...))
	{
	}
	template<typename T>
	inline Resource<T>* ResourceRef<T>::GetResource() const
	{
		return static_cast<Resource<T>*>(ResourceBaseRef::GetResource());
	}
	template<typename T>
	inline T* ResourceRef<T>::GetValue() const
	{
		Resource<T>* resource = static_cast<Resource<T>*>(ResourceBaseRef::GetResource());

		if (resource == nullptr)
			return nullptr;

		return &resource->Get();
	}
	template<typename T>
	inline T& ResourceRef<T>::operator*()
	{
		return static_cast<Resource<T>*>(ResourceBaseRef::GetResource())->Get();
	}
	template<typename T>
	inline const T& ResourceRef<T>::operator*() const
	{
		return static_cast<Resource<T>*>(ResourceBaseRef::GetResource())->Get();
	}
	template<typename T>
	inline T* ResourceRef<T>::operator->()
	{
		return &static_cast<Resource<T>*>(ResourceBaseRef::GetResource())->Get();
	}
	template<typename T>
	inline const T* ResourceRef<T>::operator->() const
	{
		return &static_cast<Resource<T>*>(ResourceBaseRef::GetResource())->Get();
	}
}