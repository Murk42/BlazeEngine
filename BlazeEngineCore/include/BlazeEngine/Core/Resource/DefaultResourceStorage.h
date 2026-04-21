#pragma once
#include "BlazeEngine/Core/Resource/ResourceStorage.h"
#include "BlazeEngine/Core/Container/Map.h"
#include "BlazeEngine/Core/Container/List.h"
#include "BlazeEngine/Core/Debug/Logger.h"

namespace Blaze
{
	template<typename T>
	class Resource;

	template<typename T>
	class BLAZE_API DefaultResourceStorage : public ResourceStorage<T>
	{
	public:
		ResourceRef<T> AllocateNamedResource(StringView name) override;
		ResourceRef<T> AllocateResource() override;
		ResourceRef<T> GetNamedResource(StringView name) override;
	private:
		Map<String, Resource<T>> resources;
		List<Resource<T>> unnamedResources;

		void FreeResource(Resource<T>& resource) override;
	};

	template<typename T>
	inline ResourceRef<T> DefaultResourceStorage<T>::AllocateNamedResource(StringView name)
	{
		return resources.Insert(name, *this).iterator->value;
	}
	template<typename T>
	inline ResourceRef<T> DefaultResourceStorage<T>::AllocateResource()
	{		
		return *unnamedResources.AddFront(*this);
	}
	template<typename T>
	inline ResourceRef<T> DefaultResourceStorage<T>::GetNamedResource(StringView name)
	{
		auto it = resources.Find(name);

		if (it.IsNull())
			return { };

		return it->value;
	}
	template<typename T>
	inline void DefaultResourceStorage<T>::FreeResource(Resource<T>& resource)
	{
		for (auto it = resources.FirstIterator(); it != resources.BehindIterator(); ++it)
			if (&it->value == &resource)
			{
				resources.Erase(it);
				return;
			}

		if (unnamedResources.EraseOne([&](Resource<T>& ref) -> bool
			{
				return &ref == &resource;
			}))
			return;		

		BLAZE_LOG_ERROR("Trying to free an invalid resource");
	}
}