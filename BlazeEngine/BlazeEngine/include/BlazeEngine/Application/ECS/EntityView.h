#pragma once
#include "BlazeEngineCore/Common/Tuple.h"
#include "BlazeEngine/Application/ECS/Entity.h"

namespace Blaze::ECS
{
	template<typename ... Cs> requires (IsComponent<Cs> && ...)
	class EntityView
	{		
		Tuple<Cs*...> components;
		Entity* entity;
	public:
		EntityView()
			: entity(nullptr), components(((Cs*)nullptr, ...))
		{
		}
		EntityView(Entity* entity, const Tuple<Cs*...>& components)
			: entity(entity), components(components)
		{
		}			

		Entity* GetEntity() const 
		{ 
			return entity; 
		}

		template<typename C> requires (TypeGroup<Cs...>::template HasType<C>)
		C* GetComponent()
		{
			return components.GetValueByType<C*>();
		}
		template<uintMem I> requires (I < sizeof...(Cs))
		auto* GetComponent()
		{
			return components.GetValueByIndex<I>();			
		}
	};
}