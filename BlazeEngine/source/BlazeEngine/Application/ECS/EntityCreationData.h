#pragma once

namespace Blaze::ECS
{
	class Entity;
	class Component;
	class Scene;
	class System;
	class ComponentTypeData;
	
	const ComponentTypeData** GetEntityComponentsTypeData(const Entity* entity);
	Component** GetEntityComponents(const Entity* entity);
	
	struct EntityCreationData
	{		
		Scene* scene = nullptr;		

		
		ArrayView<const ComponentTypeData*> typesData;				
		const ComponentTypeData** currentTypeData;
		Entity* currentEntity = nullptr;

		System** systems = nullptr;		
	};
	
	extern List<EntityCreationData> entityCreationData;
	extern EntityCreationData* currentEntityCreationData;
}