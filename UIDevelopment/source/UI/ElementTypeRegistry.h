#pragma once

namespace Blaze::UI2
{
	struct ElementTypeData
	{
		String name;
		String fileName;

		size_t size;
		void(*construct)(void*);
		void(*destruct)(void*);

		size_t managerSize;
		void(*constructManager)(void*);
		void(*destructManager)(void*);
	};

	class ElementTypeRegistry
	{
		std::vector<ElementTypeData> types;
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

		Result RegisterType(StringView, 
			size_t, void(*)(void*), void(*)(void*),
			size_t, void(*)(void*), void(*)(void*));

		Result OverrideManager(StringView,
			size_t, void(*)(void*), void(*)(void*));
	public:
		ElementTypeRegistry();
		ElementTypeRegistry(const ElementTypeRegistry&);
		~ElementTypeRegistry();

		template<typename T>
		Result RegisterType()
		{
			return RegisterType(
				T::typeName,
				sizeof(T), Construct<T>, Destruct<T>,
				sizeof(typename T::ManagerType), Construct<typename T::ManagerType>, Destruct<typename T::ManagerType>);
		}
		template<typename T, typename M>
		Result RegisterType()
		{
			return RegisterType(
				T::typeName, T::fileName,
				sizeof(T), Construct<T>, Destruct<T>,
				sizeof(M), Construct<M>, Destruct<M>);
		}
		
		template<typename T, typename M>
		Result OverrideManager()
		{
			return OverrideManager(
				T::typeName,
				sizeof(M), Construct<M>, Destruct<M>);
		}

		void RegisterCoreTypes();

		template<typename T>
		uint GetElementTypeIndex() const
		{
			return GetElementTypeIndex(T::typeName);
		}
		int GetElementTypeIndex(StringView name) const;

		uint GetElementTypeCount() const;
		ElementTypeData GetElementTypeData(uint index) const;

		bool IsValidTypeIndex(uint index) const;

		static ElementTypeRegistry CoreRegistry();
	};
}