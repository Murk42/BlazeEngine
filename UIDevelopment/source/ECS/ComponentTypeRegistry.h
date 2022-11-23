#pragma once

namespace Blaze::ECS
{
	struct ComponentTypeData
	{
		StringView typeName;

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
		static ComponentTypeData GetTypeData()
		{
			return { T::typeName, sizeof(T), Construct<T>, Destruct<T> };
		}
	};

	class ComponentTypeRegistry
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