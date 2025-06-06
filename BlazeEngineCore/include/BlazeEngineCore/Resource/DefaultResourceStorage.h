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
		Resource<T>& CreateResource(StringView name) override;
		Resource<T>* GetResource(StringView name) override;
	private:
		Map<StringView, Resource<T>> resources;
	};

	template<std::constructible_from<> T>
	inline Resource<T>& DefaultResourceStorage<T>::CreateResource(StringView name)
	{
		auto [iterator, inserted] = resources.Insert(name);		

		return iterator->value;
	}
	template<std::constructible_from<> T>
	inline Resource<T>* DefaultResourceStorage<T>::GetResource(StringView name)
	{
		auto it = resources.Find(name);

		if (it.IsNull())
			return nullptr;

		return &it->value;	
	}
}