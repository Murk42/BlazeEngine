#pragma once
#include <span>
#include <unordered_map>

namespace Blaze::ECS
{
	class Component;
	class System;
	class Entity;
	class Manager;

	struct ComponentTypeData;
	class ComponentTypeRegistry;

	class BLAZE_API ComponentTypeData
	{		
	public:		
		ComponentTypeData();
		ComponentTypeData(const ComponentTypeData&) = delete;
		ComponentTypeData(ComponentTypeData&&) noexcept;

		inline const ComponentTypeRegistry* GetRegistry() const { return registry; }
		inline StringView GetTypeName() const { return typeName; }
		inline uint Index() const { return index; }

		inline size_t Size() const { return size; }
		inline size_t BaseOffset() const { return baseOffset; }

		inline void Construct(Component* ptr) const { constructor((uint8*)ptr - baseOffset); }
		inline void ConstructDirect(void* ptr) const { constructor(ptr); }
		inline void Destruct(Component* ptr) const { destructor((uint8*)ptr - baseOffset); }
		inline void DestructDirect(void* ptr) const { destructor(ptr); }

		inline size_t SystemSize() const { return systemSize; }
		inline size_t SystemBaseOffset() const { return systemBaseOffset; }

		inline void ConstructSystem(System* ptr) const { systemConstructor((uint8*)ptr - baseOffset); }
		inline void ConstructSystemDirect(void* ptr) const { systemConstructor(ptr); }
		inline void DestructSystem(System* ptr) const { systemDestructor((uint8*)ptr - baseOffset); }
		inline void DestructSystemDirect(void* ptr) const { systemDestructor(ptr); }

		inline bool IsNone() const { return typeName == nullptr; }

		operator bool() const { return !IsNone(); }

		ComponentTypeData& operator=(const ComponentTypeData&) = delete;		
		ComponentTypeData& operator=(ComponentTypeData&&) noexcept;

		bool operator==(const ComponentTypeData&) const;
		bool operator!=(const ComponentTypeData&) const;
	private:
		using Constructor = void(*)(void*);
		using Destructor = void(*)(void*);
		using SystemConstructor = void(*)(void*);
		using SystemDestructor = void(*)(void*);

		ComponentTypeData(ComponentTypeRegistry* registry, const char* name, uint index,
			size_t size, ptrdiff_t baseOffset, Constructor constructor, Destructor destructor,
			size_t systemSize, ptrdiff_t systemBaseOffset, SystemConstructor systemConstructor, SystemConstructor systemDestructor);

		const ComponentTypeRegistry* registry;
		const char* typeName;
		uint index;

		size_t size;
		ptrdiff_t baseOffset;
		Constructor constructor;
		Destructor destructor;

		size_t systemSize;
		ptrdiff_t systemBaseOffset;
		SystemConstructor systemConstructor;
		SystemDestructor systemDestructor;

		friend class ComponentTypeRegistry;
	};

	class BLAZE_API ComponentTypeRegistry
	{		
	public:
		template<typename ... T>
		static ComponentTypeRegistry NewRegistry();

		ComponentTypeRegistry(const ComponentTypeRegistry&);
		ComponentTypeRegistry(ComponentTypeRegistry&&) noexcept;
		~ComponentTypeRegistry();

		template<typename T>
		const ComponentTypeData& GetComponentTypeData() const;
		const ComponentTypeData& GetComponentTypeData(StringView name) const;

		std::span<const ComponentTypeData> GetAllTypesData() const;

		ComponentTypeRegistry& operator=(const ComponentTypeRegistry&);
		ComponentTypeRegistry& operator=(ComponentTypeRegistry&&) noexcept;

	private:
		ComponentTypeRegistry();

		ComponentTypeData* types;
		uint typeCount;
		std::unordered_map<StringView, uint> nameTable;

		template<typename T> static void Construct(void* ptr);
		template<typename T> static void Destruct(void* ptr);
		template<typename T> static void ConstructSystem(void* ptr);
		template<typename T> static void DestructSystem(void* ptr);

		Result RegisterType(const char* name,
			size_t size, ptrdiff_t baseOffset, ComponentTypeData::Constructor constructor, ComponentTypeData::Destructor destructor,
			size_t systemSize, ptrdiff_t systemBaseOffset, ComponentTypeData::SystemConstructor systemConstructor, ComponentTypeData::SystemDestructor systemDestructor);

		template<typename T>
		Result RegisterType();

	};

	template<typename ... T>
	static ComponentTypeRegistry ComponentTypeRegistry::NewRegistry()
	{
		ComponentTypeRegistry registry;
		(registry.RegisterType<T>(), ...);
		return std::move(registry);
	}

	template<typename T>
	Result ComponentTypeRegistry::RegisterType()
	{
		using System = typename T::System;
		return RegisterType(T::typeName,
			sizeof(T), BaseOffset<Component, T>(), Construct<T>, Destruct<T>,
			sizeof(System), BaseOffset<ECS::System, System>(), ConstructSystem<System>, DestructSystem<System>);
	}

	template<typename T>
	const ComponentTypeData& ComponentTypeRegistry::GetComponentTypeData() const
	{
		return GetComponentTypeData(T::typeName);
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