#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/Resource/Resource.h"

namespace Blaze
{
	class ResourceBase;
	template<typename T>
	class Resource;

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

		operator bool() const;

		ResourceBase& GetResourceBase();
		const ResourceBase& GetResourceBase() const;

		template<typename T>
		Resource<T>* TryGetResource();
		template<typename T>
		const Resource<T>* TryGetResource() const;
		template<typename T>
		Resource<T>& GetResource();
		template<typename T>
		const Resource<T>& GetResource() const;

		template<typename T>
		T* TryGet();
		template<typename T>
		const T* TryGet() const;
		template<typename T>
		T& Get();
		template<typename T>
		const T& Get() const;

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
		ResourceRef(Resource<T>& resource);
		ResourceRef(const ResourceRef& other);
		ResourceRef(ResourceRef&& other) noexcept;
		~ResourceRef();

		Resource<T>& GetResource();
		const Resource<T>& GetResource() const;

		T& Get();
		const T& Get() const;

		T& operator*();
		const T& operator*() const;
		T* operator->();
		const T* operator->() const;				

		bool operator==(const ResourceRef& other) const;
		bool operator!=(const ResourceRef& other) const;

		ResourceRef& operator=(const ResourceRef& other);
		ResourceRef& operator=(ResourceRef&& other) noexcept;
	};

	template<typename T>
	inline Resource<T>* ResourceBaseRef::TryGetResource()
	{
		auto* resource = dynamic_cast<Resource<T>*>(resourceBase);

		if (resource == nullptr)
			return nullptr;

		return resource;
	}
	template<typename T>
	inline const Resource<T>* ResourceBaseRef::TryGetResource() const
	{
		auto* resource = dynamic_cast<const Resource<T>*>(resourceBase);

		if (resource == nullptr)
			return nullptr;

		return resource;
	}
	template<typename T>
	inline Resource<T>& ResourceBaseRef::GetResource()
	{
		auto* resource = dynamic_cast<Resource<T>*>(resourceBase);
		
		if (resource == nullptr)
			BLAZE_LOG_FATAL("Invalid access to a resource");

		return *resource;
	}
	template<typename T>
	inline const Resource<T>& ResourceBaseRef::GetResource() const
	{
		auto* resource = dynamic_cast<const Resource<T>*>(resourceBase);

		if (resource == nullptr)
			BLAZE_LOG_FATAL("Invalid access to a resource");

		return *resource;
	}
	template<typename T>
	inline T* ResourceBaseRef::TryGet()
	{
		Resource<T>* resource = dynamic_cast<Resource<T>*>(resourceBase);

		if (resource == nullptr)
			return nullptr;

		return &resource->Get();
	}
	template<typename T>
	inline const T* ResourceBaseRef::TryGet() const
	{
		const Resource<T>* resource = dynamic_cast<const Resource<T>*>(resourceBase);

		if (resource == nullptr)
			return nullptr;			

		return &resource->Get();
	}
	template<typename T>
	inline T& ResourceBaseRef::Get()
	{
		Resource<T>* resource = dynamic_cast<Resource<T>*>(resourceBase);

		if (resource == nullptr)
			BLAZE_LOG_FATAL("Invalid access to a resource");

		return resource->Get();
	}
	template<typename T>
	inline const T& ResourceBaseRef::Get() const
	{
		const Resource<T>* resource = dynamic_cast<const Resource<T>*>(resourceBase);

		if (resource == nullptr)
			BLAZE_LOG_FATAL("Invalid access to a resource");

		return resource->Get();
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
	inline ResourceRef<T>::ResourceRef(const ResourceRef& other)
		: ResourceBaseRef(other)
	{
	}
	template<typename T>
	inline ResourceRef<T>::ResourceRef(ResourceRef&& other) noexcept
		: ResourceBaseRef(std::move(other))
	{
	}
	template<typename T>
	inline ResourceRef<T>::~ResourceRef()
	{
	}
	template<typename T>
	inline T& ResourceRef<T>::Get()
	{
		return ((Resource<T>&)GetResourceBase()).Get();
	}
	template<typename T>
	inline const T& ResourceRef<T>::Get() const
	{
		return ((const Resource<T>&)GetResourceBase()).Get();
	}
	template<typename T>
	inline Resource<T>& ResourceRef<T>::GetResource()
	{
		return (Resource<T>&)GetResourceBase();
	}
	template<typename T>
	inline const Resource<T>& ResourceRef<T>::GetResource() const
	{
		return (const Resource<T>&)GetResourceBase();
	}
	template<typename T>
	inline T& ResourceRef<T>::operator*()
	{
		return Get();
	}
	template<typename T>
	inline const T& ResourceRef<T>::operator*() const
	{
		return Get();
	}
	template<typename T>
	inline T* ResourceRef<T>::operator->()
	{
		return &Get();
	}
	template<typename T>
	inline const T* ResourceRef<T>::operator->() const
	{
		return &Get();
	}
	template<typename T>
	inline bool ResourceRef<T>::operator==(const ResourceRef& other) const
	{
		return ResourceBaseRef::operator==(other);
	}
	template<typename T>
	inline bool ResourceRef<T>::operator!=(const ResourceRef& other) const
	{
		return ResourceBaseRef::operator!=(other);
	}
	template<typename T>
	inline auto ResourceRef<T>::operator=(const ResourceRef& other) -> ResourceRef&
	{
		ResourceBaseRef::operator=(other);
		return *this;
	}
	template<typename T>
	inline auto ResourceRef<T>::operator=(ResourceRef&& other) noexcept -> ResourceRef&
	{
		ResourceBaseRef::operator=(std::move(other));
		return *this;
	}	
}