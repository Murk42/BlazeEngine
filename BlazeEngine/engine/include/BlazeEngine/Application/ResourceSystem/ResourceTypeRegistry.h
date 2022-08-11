#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Core/Result.h"
#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/DataStructures/StringHash.h"
#include "BlazeEngine/DataStructures/Common.h"
#include <unordered_map>

namespace Blaze
{
	namespace Resource
	{				
		struct ResourceTypeData
		{
			String name;

			size_t size;
			void(*construct)(void*);
			void(*destruct)(void*);			
		};

		class BLAZE_API ResourceTypeRegistry
		{
			std::vector<ResourceTypeData> types;
			std::unordered_map<String, uint> nameTable;

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

			Result RegisterType(StringView, size_t, void(*)(void*), void(*)(void*));			
		public:
			ResourceTypeRegistry();
			ResourceTypeRegistry(const ResourceTypeRegistry&);
			~ResourceTypeRegistry();

			template<typename T>
			Result RegisterType()
			{				
				return RegisterType(T::typeName, sizeof(T), Construct<T>, Destruct<T>);
			}

			void RegisterCoreTypes();
			
			template<class T>
			uint GetTypeIndex() 
			{										
				return GetTypeIndex(T::typeName);
			}			
			uint GetTypeIndex(StringView name);

			uint GetResourceTypeCount() const;
			const ResourceTypeData& GetResourceTypeData(uint index) const;

			static ResourceTypeRegistry CoreRegistry();
		};
	}
}