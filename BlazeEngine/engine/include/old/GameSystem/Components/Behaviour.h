#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/GameSystem/ECS/Component.h"
#include "BlazeEngine/GameSystem/ECS/System.h"

namespace Blaze
{ 
	template<typename Derived, typename ... Bases>
	class Behaviour : public Component<Derived, Bases...>
	{			
	public:
		void Update()
		{
			
		}

		class BehaviourSystem : public System<BehaviourSystem, Derived>
		{
		public:
			void Update(BaseSystem::Array<Derived> behaviours)
			{
				for (auto& behaviour : behaviours)
					behaviour.Update();
			}
		};
	};	
}