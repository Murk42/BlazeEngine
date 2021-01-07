#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/GameSystem/ECS/Common.h"

namespace Blaze
{
	class ECS;
	class BaseComponent;
	class String;

	class BLAZE_API Entity
	{
		EntityID id;
		ECS* ecs;

		//NULL_COPY_AND_ASSIGN(Entity);
		Entity(EntityID, ECS*);
	public:
		Entity();
		Entity(const Entity&);

		inline EntityID GetEntityID() const { return id; }

		bool HasComponent(ComponentTypeID);
		template<typename T>
		bool HasComponent()
		{
			return HasComponent(T::GetTypeID());
		}
 
		BaseComponent* GetComponent(ComponentTypeID);
		template<typename T>
		T* GetComponent()
		{
			return (T*)GetComponent(T::GetTypeID());
		}

		BaseComponent* AddComponent(ComponentTypeID);
		template<typename T>
		T* AddComponent()
		{
			return (T*)AddComponent(T::GetTypeID());
		}

		void RemoveComponent(ComponentTypeID);
		template<typename T>
		void RemoveComponent()
		{
			RemoveComponent(T::GetTypeID());
		}

		const String& GetName() const;
		void SetName(String&&);

		void operator=(const Entity&);

		friend class BaseComponent;
		friend class ECS;
	};
}