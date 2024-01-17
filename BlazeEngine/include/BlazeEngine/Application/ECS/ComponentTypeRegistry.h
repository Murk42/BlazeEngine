#pragma once

namespace Blaze::ECS
{
	class Component;
	class System;
	class Entity;
	class Manager;

	class ComponentTypeData;
	class ComponentTypeRegistry;	

	template<typename T>
	concept IsSystem = requires {
		std::derived_from<T, System>;
	};

	template<typename T>
	concept IsComponent =  requires {
		std::derived_from<T, Component>;
		T();									
		T::typeName;
		IsSystem<typename T::System>;		
		typename T::TypeTags;
	};
	
	class BLAZE_API ComponentTypeData
	{		
	public:				
		using Constructor = void(*)(void*);
		using Destructor = void(*)(void*);
		using SystemConstructor = void(*)(void*);
		using SystemDestructor = void(*)(void*);		

		ComponentTypeData(ComponentTypeRegistry* registry, StringView name, uint index,
			uintMem size, ptrdiff_t baseOffset, Constructor constructor, Destructor destructor,
			uintMem systemSize, ptrdiff_t systemBaseOffset, SystemConstructor systemConstructor, SystemConstructor systemDestructor,
			Set<StringView> typeTags);
		ComponentTypeData(const ComponentTypeData&) = delete;

		inline const ComponentTypeRegistry* GetRegistry() const { return registry; }
		inline StringView GetTypeName() const { return typeName; }
		inline uint Index() const { return index; }

		inline uintMem Size() const { return size; }
		inline uintMem BaseOffset() const { return baseOffset; }

		inline void Construct(Component* ptr) const { constructor((uint8*)ptr - baseOffset); }
		inline void ConstructDirect(void* ptr) const { constructor(ptr); }
		inline void Destruct(Component* ptr) const { destructor((uint8*)ptr - baseOffset); }
		inline void DestructDirect(void* ptr) const { destructor(ptr); }

		inline uintMem SystemSize() const { return systemSize; }
		inline uintMem SystemBaseOffset() const { return systemBaseOffset; }

		inline void ConstructSystem(System* ptr) const { systemConstructor((uint8*)ptr - systemBaseOffset); }
		inline void ConstructSystemDirect(void* ptr) const { systemConstructor(ptr); }
		inline void DestructSystem(System* ptr) const { systemDestructor((uint8*)ptr - systemBaseOffset); }
		inline void DestructSystemDirect(void* ptr) const { systemDestructor(ptr); }

		inline bool IsNone() const { return typeName.Empty(); }

		inline const Set<StringView>& GetTypeTags() const { return typeTags; }

		operator bool() const { return !IsNone(); }

		ComponentTypeData& operator=(const ComponentTypeData&) = delete;		

		bool operator==(const ComponentTypeData&) const;
		bool operator!=(const ComponentTypeData&) const;
	private:
		const ComponentTypeRegistry* registry;
		StringView typeName;
		Set<StringView> typeTags;
		uint index;

		uintMem size;
		ptrdiff_t baseOffset;
		Constructor constructor;
		Destructor destructor;

		uintMem systemSize;
		ptrdiff_t systemBaseOffset;
		SystemConstructor systemConstructor;
		SystemDestructor systemDestructor;

		friend class ComponentTypeRegistry;
	};

	class BLAZE_API ComponentTypeRegistry
	{		
	public:
		template<typename ... T> requires (IsComponent<T> && ...)
		static ComponentTypeRegistry NewRegistry();

		ComponentTypeRegistry(const ComponentTypeRegistry&);
		ComponentTypeRegistry(ComponentTypeRegistry&&) noexcept;
		~ComponentTypeRegistry();

		template<typename T>  requires IsComponent<T>
		bool GetComponentTypeData(const ComponentTypeData*& typeData) const;
		bool GetComponentTypeData(StringView name, const ComponentTypeData*& typeData) const;

		template<typename T> requires IsComponent<T>
		bool HasComponentTypeData() const; 		
		bool HasComponentTypeData(StringView name) const;

		ArrayView<ComponentTypeData> GetAllTypesData() const;

		ComponentTypeRegistry& operator=(const ComponentTypeRegistry&);
		ComponentTypeRegistry& operator=(ComponentTypeRegistry&&) noexcept;
	private:
		ComponentTypeRegistry();

		Array<ComponentTypeData> types;		
		Map<StringView, ComponentTypeData*> nameTable;

		template<typename T> static void Construct(void* ptr);
		template<typename T> static void Destruct(void* ptr);
		template<typename T> static void ConstructSystem(void* ptr);
		template<typename T> static void DestructSystem(void* ptr);

		void ReserveTypeCount(uint count);

		Result AddType(StringView name,
			uintMem size, ptrdiff_t baseOffset, ComponentTypeData::Constructor constructor, ComponentTypeData::Destructor destructor,
			uintMem systemSize, ptrdiff_t systemBaseOffset, ComponentTypeData::SystemConstructor systemConstructor, ComponentTypeData::SystemDestructor systemDestructor,
			Array<StringView> customData);

		template<typename T> requires IsComponent<T>
		Result AddType();		

		template<typename T>
		Array<StringView> GetTypeTags();

		template<uintMem index, typename G, typename A>
		void SetTypeTags(A& arr)
		{
			using Next = typename G::template Next;

			arr[index] = G::tag;

			if constexpr (G::Size > 1)
				SetTypeTags<index + 1, Next>(arr);
		}
	};

	template<typename ... T> requires (IsComponent<T> && ...)
	static ComponentTypeRegistry ComponentTypeRegistry::NewRegistry()
	{
		ComponentTypeRegistry registry;
		registry.ReserveTypeCount(sizeof...(T));
		(registry.AddType<T>(), ...);
		return std::move(registry);
	}

	template<typename T>  requires IsComponent<T>
	Result ComponentTypeRegistry::AddType()
	{
		using System = typename T::System;
		return AddType(T::typeName,
			sizeof(T), BaseOffset<Component, T>(), Construct<T>, Destruct<T>,
			sizeof(System), BaseOffset<ECS::System, System>(), ConstructSystem<System>, DestructSystem<System>,
			GetTypeTags<T>());
	}

	template<typename T>
	inline Array<StringView> ComponentTypeRegistry::GetTypeTags()
	{
		using TypeTags = typename T::template TypeTags;

		std::array<StringView, TypeTags::Size> arr = TypeTags::tags;

		return ArrayView<StringView>(arr.data(), arr.size());
	}	

	template<typename T>  requires IsComponent<T>
	bool ComponentTypeRegistry::GetComponentTypeData(const ComponentTypeData*& typeData) const
	{
		return GetComponentTypeData(T::typeName, typeData);
	}

	template<typename T> requires IsComponent<T>
	inline bool ComponentTypeRegistry::HasComponentTypeData() const
	{
		return HasComponentTypeData(T::typeName);
	}

	template<typename T>
	void ComponentTypeRegistry::Construct(void* ptr)
	{
		ptrdiff_t offset = BaseOffset<Component, T>();
		new (ptr) T();
	}
	template<typename T>
	void ComponentTypeRegistry::Destruct(void* ptr)
	{
		ptrdiff_t offset = BaseOffset<Component, T>();
		((T*)ptr)->~T();
	}
	template<typename T>
	void ComponentTypeRegistry::ConstructSystem(void* ptr)
	{
		ptrdiff_t offset = BaseOffset<System, T>();
		new (ptr) T();
	}
	template<typename T>
	void ComponentTypeRegistry::DestructSystem(void* ptr)
	{
		ptrdiff_t offset = BaseOffset<System, T>();
		((T*)ptr)->~T();
	}
}