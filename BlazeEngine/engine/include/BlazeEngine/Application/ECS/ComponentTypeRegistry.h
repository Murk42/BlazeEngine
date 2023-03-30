#pragma once
#include <vector>
#include <unordered_map>

namespace Blaze::ECS
{
	class Component;

	struct ComponentTypeData
	{
		StringView typeName;

		size_t size = 0;
		ptrdiff_t baseOffset = 0;
		void(*construct)(void*) = nullptr;
		void(*destruct)(void*) = nullptr;

		size_t systemSize = 0;
		void(*constructSystem)(void*) = nullptr;
		void(*destructSystem)(void*) = nullptr;

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
		static ComponentTypeData GetTypeData()
		{
			using S = T::System;
			return { 
				T::typeName, 
				sizeof(T), 
				BaseOffset<Component, T>(),
				Construct<T>,
				Destruct<T>,
				sizeof(S),
				Construct<S>,
				Destruct<S>
			};
		}
	};

	class BLAZE_API ComponentTypeRegistry
	{
		std::vector<ComponentTypeData> types;
		std::unordered_map<String, uint> nameTable;

	public:
		ComponentTypeRegistry();
		ComponentTypeRegistry(const ComponentTypeRegistry&);
		ComponentTypeRegistry(ComponentTypeRegistry&&) noexcept;
		~ComponentTypeRegistry();

		template<typename T>
		Result RegisterType()
		{
			return RegisterType(ComponentTypeData::GetTypeData<T>());
		}
		Result RegisterType(ComponentTypeData data);

		template<typename T>
		uint GetComponentTypeIndex() const
		{
			return GetComponentTypeIndex(T::typeName);
		}
		int GetComponentTypeIndex(StringView name) const;

		uint GetComponentTypeCount() const;
		ComponentTypeData GetComponentTypeData(uint index) const;

		bool IsValidTypeIndex(uint index) const;

		ComponentTypeRegistry& operator=(const ComponentTypeRegistry&);
		ComponentTypeRegistry& operator=(ComponentTypeRegistry&&) noexcept;
	};
}