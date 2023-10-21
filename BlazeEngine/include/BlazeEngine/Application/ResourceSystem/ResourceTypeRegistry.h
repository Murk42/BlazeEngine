#pragma once
#include "BlazeEngine/Application/ResourceSystem/Resource.h"

namespace Blaze::ResourceSystem
{
	struct ResourceTypeData
	{
		String typeName;

		size_t size;
		void(*construct)(void*); 
		void(*destruct)(void*);

		template<typename T>
		static void Construct(void* ptr)
		{
			new (ptr) T();
		}
		template<typename T>
		static void Destruct(void* ptr)
		{
			((T*)ptr)->~T();
		}

		template<typename T>
		static ResourceTypeData GetTypeData()
		{			
			return {
				T::typeName,
				sizeof(T),
				Construct<T>,
				Destruct<T>,
			};
		}
	};		

	class BLAZE_API ResourceTypeRegistry
	{		
	public:
		ResourceTypeRegistry();
		ResourceTypeRegistry(const ResourceTypeRegistry&);
		~ResourceTypeRegistry();

		template<typename T>
		Result RegisterType()
		{			
			return RegisterType(ResourceTypeData::GetTypeData<T>());
		}
		Result RegisterType(ResourceTypeData data);

		void RegisterCoreTypes();

		template<class T>
		uintMem GetResourceTypeIndex() const
		{
			return GetResourceTypeIndex(T::typeName);
		}
		uintMem GetResourceTypeIndex(StringView name) const;

		uintMem GetResourceTypeCount() const;
		ResourceTypeData GetResourceTypeData(uintMem index) const;

		bool IsValidTypeIndex(uintMem index) const;

		static ResourceTypeRegistry CoreRegistry();

	private:
		Array<ResourceTypeData> types;
		Map<String, uintMem> nameTable;
	};
}
