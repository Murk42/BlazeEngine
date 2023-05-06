#pragma once

namespace Blaze::ECS
{
	class Entity;
	class ComponentTypeData;

	struct ComponentConstructData
	{
		Entity* entity = nullptr;		
		uint state;
	};

	extern ComponentConstructData componentConstructData;
}