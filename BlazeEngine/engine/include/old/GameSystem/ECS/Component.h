#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/GameSystem/ECS/Entity.h"
#include "BlazeEngine/DataStructures/TypeTraits.h"
#include "Common.h"
#include <vector>
#include <tuple>

namespace Blaze
{
	class BLAZE_API BaseComponent
	{	
		template<typename ... T>
		using Func = void(*)(void*, T...);		
		
		EntityID entityID;
		ECS* ecs;
		ComponentTypeID typeID;		

		void operator=(const BaseComponent&) = delete;

		static ComponentTypeID RegisterComponentType(Func<>, Func<>, size_t, size_t, size_t, Func<const void*>, std::initializer_list<std::pair<size_t, size_t>>);

		const void* GetBase(size_t) const;
		void* GetBase(size_t);
	public:		
		BaseComponent(ComponentTypeID);
		BaseComponent(const BaseComponent&);

		inline EntityID GetEntityID() const { return entityID; }
		inline ComponentTypeID GetTypeID() const { return typeID; }
		inline Entity GetEntity() const { return Entity(entityID, ecs); }		

		template<typename T>
		inline const T* GetBase() const
		{
			return (T*)GetBase(typeid(T).hash_code());
		}
		template<typename T>
		inline T* GetBase()
		{
			return (T*)GetBase(typeid(T).hash_code());
		}

		template<typename, typename ...>
		friend struct Component;
		template<typename, typename ...>
		friend class System;
		friend class Entity;
		friend class ECS;
	};

	template<typename Derived, typename ... Bases>
	class Component : public BaseComponent
	{
		inline static void Construct(void* comp) { new (comp)Derived(); }
		inline static void Copy(void* comp1, const void* comp2) { new(comp1)Derived(*(const Derived*)comp2); }
		inline static void Destruct(void* comp) { ((Derived*)comp)->~Derived(); }

		struct Init
		{
			ComponentTypeID typeID;
			Init()
				: typeID(BaseComponent::RegisterComponentType(Construct, Destruct, sizeof(Derived), baseoffset(Derived, BaseComponent),
					typeid(Derived).hash_code(), Copy, { {typeid(Bases).hash_code(), baseoffset(Derived, Bases) }... }))
			{

			}
		} static init;
	public:

		Component() : BaseComponent(init.typeID) { }	
		
		inline static ComponentTypeID GetTypeID() { return init.typeID; }
	};
}