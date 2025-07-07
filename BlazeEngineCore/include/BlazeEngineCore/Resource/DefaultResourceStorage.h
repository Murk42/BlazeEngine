#pragma once
#include "BlazeEngineCore/Resource/ResourceStorage.h"
#include "BlazeEngineCore/DataStructures/Map.h"
#include "BlazeEngineCore/Debug/Logger.h"

namespace Blaze
{
	template<std::constructible_from<> T>
	class BLAZE_CORE_API DefaultResourceStorage : public ResourceStorage<T>
	{
	public:
		ResourceRef<T> CreateResource(StringView name) override;
		ResourceRef<T> GetResource(StringView name) override;
	private:
		Map<String, Resource<T>> resources;
		List<Resource<T>> unnamedResources;
	};

	template<std::constructible_from<> T>
	inline ResourceRef<T> DefaultResourceStorage<T>::CreateResource(StringView name)
	{
		if (name.Empty())
			return *unnamedResources.AddFront();
		else		
			return resources.Insert(name).iterator->value;
	}
	template<std::constructible_from<> T>
	inline ResourceRef<T> DefaultResourceStorage<T>::GetResource(StringView name)
	{
		auto it = resources.Find(name);

		if (it.IsNull())
			return { };

		return it->value;	
	}
}