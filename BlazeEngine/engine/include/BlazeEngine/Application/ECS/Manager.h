#pragma once
#include "BlazeEngine/DataStructures/Array.h"
#include "BlazeEngine/Application/ResourceSystem/ResourceManager.h"
#include "ComponentTypeRegistry.h"
#include "System.h"

namespace Blaze::ECS
{
	class BLAZE_API Manager
	{
		ComponentTypeRegistry registry;
		Array<System*> systems;
	public:
		ResourceSystem::ResourceManager* resourceManager;

		Manager();
		~Manager();

		Result SetRegistry(const ComponentTypeRegistry& registry);

		template<typename T>
		typename T::System* GetSystem() const
		{
			return (typename T::System*)GetSystem(registry.GetComponentTypeIndex<T>());
		}
		System* GetSystem(uint typeIndex) const;

		inline const ComponentTypeRegistry& GetRegistry() const { return registry; }
	};
}