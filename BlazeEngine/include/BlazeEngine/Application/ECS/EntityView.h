#pragma once
#include "BlazeEngine/Application/ECS/Entity.h"
#include "BlazeEngine/DataStructures/TemplateGroup.h"

namespace Blaze::ECS
{
	template<typename ... Cs> requires (IsComponent<Cs> && ...)
	class EntityView
	{		
		Entity* entity;
	public:
		EntityView()
			: entity(nullptr)
		{
		}		
		EntityView(Entity* entity)
			: entity(entity)
		{
		}

		Entity* GetEntity() const 
		{ 
			return entity; 
		}

		template<uint I> requires (I < sizeof...(Cs))
		auto GetComponent()
		{
			using R = decltype((typename TemplateGroup<Cs...>::template TypeAtIndex<I>*)entity->GetComponent(I));
			if (entity == nullptr)
				return (R)nullptr;

			return (typename TemplateGroup<Cs...>::template TypeAtIndex<I>*)entity->GetComponent(I);
		}
	};
}