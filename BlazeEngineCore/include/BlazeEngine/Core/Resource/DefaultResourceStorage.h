#pragma once
#include "BlazeEngine/Core/Resource/ResourceStorage.h"
#include "BlazeEngine/Core/Container/Map.h"
#include "BlazeEngine/Core/Debug/Logger.h"

namespace Blaze
{
	template<IsConstructibleFrom<> T>
	class BLAZE_API DefaultResourceStorage : public ResourceStorage<T>
	{
	public:
		ResourceRef<T> CreateResource(StringView name) override;
		ResourceRef<T> GetResource(StringView name) override;
	private:
		Map<String, Resource<T>> resources;
		List<Resource<T>> unnamedResources;
	};

	template<IsConstructibleFrom<> T>
	inline ResourceRef<T> DefaultResourceStorage<T>::CreateResource(StringView name)
	{
		if (name.Empty())
			return *unnamedResources.AddFront();
		else
			return resources.Insert(name).iterator->value;
	}
	template<IsConstructibleFrom<> T>
	inline ResourceRef<T> DefaultResourceStorage<T>::GetResource(StringView name)
	{
		auto it = resources.Find(name);

		if (it.IsNull())
			return { };

		return it->value;
	}
}