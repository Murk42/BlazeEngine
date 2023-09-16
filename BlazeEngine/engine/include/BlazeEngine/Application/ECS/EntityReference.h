#pragma once
#include "BlazeEngine/Application/ECS/Scene.h"

namespace Blaze::ECS
{
	template<typename ... Cs> requires (IsComponent<Cs> && ...)
	class EntityReference
	{
		Entity* entity;
	public:
		EntityReference();
		EntityReference(Scene& scene);
		EntityReference(Entity* entity);
		//EntityReference(const EntityReference&) = default;

		void CreateNew(Scene& scene);
		void SetNew(Entity* entity);

		template<typename C> requires IsAnyOf<C, Cs...>
		C* GetComponent();

		inline Entity* GetEntity() const { return entity; }


		//EntityReference& operator=(const EntityReference&) = default;
	};

	template<typename ...Cs> requires (IsComponent<Cs> && ...)
	inline EntityReference<Cs...>::EntityReference()
		: entity(nullptr)
	{
	}

	template<typename ...Cs> requires (IsComponent<Cs> && ...)
	inline EntityReference<Cs...>::EntityReference(Scene& scene) 
		: entity(scene.Create<Cs...>().GetEntity())
	{
	}

	template<typename ...Cs> requires (IsComponent<Cs> && ...)
	inline EntityReference<Cs...>::EntityReference(Entity* entity)
	{
		this->entity = entity;
	}

	template<typename ...Cs> requires (IsComponent<Cs> && ...)
	inline void EntityReference<Cs...>::CreateNew(ECS::Scene& scene)
	{
		entity = scene.Create<Cs...>().GetEntity();
	}

	template<typename ...Cs> requires (IsComponent<Cs> && ...)
	inline void EntityReference<Cs...>::SetNew(Entity* entity)
	{
		this->entity = entity;
	}

	template<typename ...Cs> requires (IsComponent<Cs> && ...)
	template<typename C>  requires IsAnyOf<C, Cs...>
	inline C* EntityReference<Cs...>::GetComponent()
	{
		return entity->GetComponent<C>();
	}
}