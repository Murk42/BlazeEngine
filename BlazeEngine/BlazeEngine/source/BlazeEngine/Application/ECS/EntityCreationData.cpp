#include "pch.h"
#include "EntityCreationData.h"
#include "BlazeEngine/Application/ECS/Entity.h"

namespace Blaze::ECS
{	
	List<EntityCreationData> entityCreationData;
	EntityCreationData* currentEntityCreationData;

	const ComponentTypeData** GetEntityComponentsTypeData(const Entity* entity)
	{
		return (const ComponentTypeData**)((const byte*)entity + sizeof(Entity) + sizeof(Component*) * entity->GetComponentCount());
	}
	Component** GetEntityComponents(const Entity* entity)
	{
		return (Component**)((const byte*)entity + sizeof(Entity));		
	}
}