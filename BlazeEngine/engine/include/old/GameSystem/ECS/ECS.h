#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/GameSystem/ECS/Component.h"
#include "BlazeEngine/GameSystem/ECS/Common.h"
#include "BlazeEngine/DataStructures/String.h"
#include <unordered_map>

namespace Blaze
{	
	class BaseSystem;			
	class ByteStream;

	class BLAZE_API ECS
	{
		struct EntityData
		{
			struct ComponentHandle
			{
				ComponentHandle() = default;
				ComponentHandle(ComponentTypeID, size_t);
				ComponentTypeID typeID;
				size_t index;
			};
			std::vector<ComponentHandle> components;
			String name;			

			EntityID GetID(ECS*);
		};		
		struct ComponentArray
		{
			std::vector<uint8> memory;
			std::vector<BaseComponent*> components;		
		};

		std::vector<uint8> systemsMemory;
		std::vector<BaseSystem*> systems;

		std::vector<std::vector<uint8>> components;
		std::unordered_map<EntityID, EntityData> entities;

		BaseComponent* AllocateComponent(ComponentTypeID);
		BaseComponent* AllocateComponent(ComponentTypeID, const BaseComponent*);
		void FreeComponent(ComponentTypeID, size_t);

		EntityID lastEntityID;
		EntityID GetNextEntityID();

		NULL_COPY_AND_ASSIGN(ECS);
	public:
		ECS();
		~ECS();

		Entity CreateEntity(String&& name, const std::initializer_list<ComponentTypeID>& componentTypes);
		Entity CreateEntity(String&& name, const std::initializer_list<ComponentTypeID>& componentTypes, const std::initializer_list<const BaseComponent*>& components);
		template<typename ... Components>
		Entity CreateEntity(String&& name)
		{
			return CreateEntity(std::move(name), { Components::GetTypeID()... });
		}
		template<typename ... Components>
		Entity CreateEntity(String&& name, const Components& ... components)
		{ 
			return CreateEntity(std::move(name), { Components::GetTypeID()... }, { &components... });
		}
		void DeleteEntity(const Entity& entity);

		void UpdateSystems();
		void UpdateSystem(SystemTypeID typeID);		 
		template<typename T>
		void UpdateSystem()
		{
			UpdateSystem(T::GetTypeID());
		}		

		BaseSystem* GetSystem(SystemTypeID);
		template<typename T>
		T* GetSystem()
		{
			return (T*)GetSystem(T::GetTypeID());
		}

		Entity GetEntity(const String& name);

		template<typename, typename ...>
		friend class System;
		friend class BaseSystem;
		friend class Entity;
		friend class BaseComponent;
		friend class BaseScene;
	};
}