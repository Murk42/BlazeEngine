#pragma once
#include "ComponentTypeRegistry.h"

namespace Blaze::ECS
{
	class Manager
	{
		ComponentTypeRegistry registry;
	public:
		Result SetRegistry(const ComponentTypeRegistry& registry);

		inline const ComponentTypeRegistry& GetRegistry() const { return registry; }
	};
}