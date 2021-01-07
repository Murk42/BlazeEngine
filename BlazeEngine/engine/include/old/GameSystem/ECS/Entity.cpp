#include "Entity.h"
#include "BlazeEngine/GameSystem/ECS/ECS.h"
#include "BlazeEngine/GameSystem/ECS/Component.h"

namespace Blaze
{
	Entity::Entity(EntityID id, ECS* ecs)
		: id(id), ecs(ecs)
	{

	}
	Entity::Entity()
		: ecs(nullptr)
	{

	}
	Entity::Entity(const Entity& e)
		: id(e.id), ecs(e.ecs)
	{

	}

	void Entity::operator=(const Entity& e)
	{
		id = e.id;
		ecs = e.ecs;
	}

	bool Entity::HasComponent(ComponentTypeID typeID)
	{
		ECS::EntityData& entity = ecs->entities[id];
		for (const auto& c : entity.components)
			if (c.typeID == typeID)
				return true;
		return false;
	}

	BaseComponent* Entity::GetComponent(ComponentTypeID typeID)
	{
		ECS::EntityData& entity = ecs->entities[id];
		for (auto& c : entity.components)
			if (c.typeID == typeID)
				return (BaseComponent*)(ecs->components[typeID].data() + c.index);
		return nullptr;
	}

	BaseComponent* Entity::AddComponent(ComponentTypeID typeID)
	{
		ECS::EntityData& entity = ecs->entities[id];
		BaseComponent* component = ecs->AllocateComponent(typeID);

		entity.components.emplace_back(typeID, (uint8*)component - ecs->components[typeID].data());
		component->entityID = id;
		return nullptr;
	}

	void Entity::RemoveComponent(ComponentTypeID typeID)
	{
		ECS::EntityData& entity = ecs->entities[id];
		for (size_t i = 0; i < entity.components.size(); ++i)
			if (entity.components[i].typeID == typeID)
			{
				ecs->FreeComponent(entity.components[i].typeID, entity.components[i].index);
				entity.components.erase(entity.components.begin() + i);
			}
	}

	const String& Entity::GetName() const
	{
		return ecs->entities[id].name;
	}
	void Entity::SetName(String&& name)
	{
		ecs->entities[id].name = std::move(name);
	}
}