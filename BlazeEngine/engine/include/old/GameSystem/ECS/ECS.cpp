#include "ECS.h"
#include "System.h"
#include "BlazeEngine/DataStructures/ByteStream.h"
#include "BlazeEngine/Internal/TypeStructs.h"

namespace Blaze
{
	struct ComponentType;
	extern std::vector<ComponentType> componentTypes;
	extern std::vector<SystemType> systemTypes;

	ECS::EntityData::ComponentHandle::ComponentHandle(ComponentTypeID typeID, size_t index)
		: typeID(typeID), index(index)
	{

	}

	EntityID ECS::EntityData::GetID(ECS* ecs)
	{
		return ((std::pair<EntityID, EntityData>*)(this - (size_t) & ((std::pair<EntityID, EntityData>*)0)->second))->first;
	}
	 
	BaseComponent* ECS::AllocateComponent(ComponentTypeID typeID)
	{
		std::vector<uint8>& memory = components[typeID];
		const auto& type = componentTypes[typeID];		

		memory.resize(memory.size() + type.size);
		BaseComponent* base = type.ToBase(memory.end()._Ptr - type.size);

		type.construct(base);
		base->ecs = this;

		return base;
	}
	BaseComponent* ECS::AllocateComponent(ComponentTypeID id, const BaseComponent* comp)
	{
		std::vector<uint8>& memory = components[id];
		const auto& type = componentTypes[id];

		memory.resize(memory.size() + type.size);
		void* ptr = memory.end()._Ptr - type.size;
		BaseComponent* base = type.ToBase(ptr);

		type.copy(ptr, type.ToDerived(comp));
		base->ecs = this;		

		return base;
	}
	void ECS::FreeComponent(ComponentTypeID id, size_t index)
	{
		std::vector<uint8>& memory = components[id];
		const auto& type = componentTypes[id];
		BaseComponent* base = (BaseComponent*)(memory.data() + index);
		void* ptr = type.ToDerived(base);

		componentTypes[id].destruct(ptr);

		memmove(ptr, (uint8*)ptr + type.size, memory.size() - index - type.size);

		memory.resize(memory.size() - type.size);

		for (int i = index; i < memory.size(); i += type.size)
		{
			BaseComponent* base = type.ToBase(memory.data() + i);
			EntityData& entity = entities[base->entityID];
			for (auto& c : entity.components)
				if (c.typeID == id)
				{
					c.index -= type.size;
					break;
				}
		}
	}

	EntityID ECS::GetNextEntityID()
	{
		return lastEntityID++;
	}

	ECS::ECS()
		: lastEntityID(0),
		systemsMemory(), 
		systems(),
		components(),
		entities()
	{		
		components.resize(componentTypes.size());

		size_t systemSizes = 0;
		systems.resize(systemTypes.size());
		for (size_t i = 0; i < systems.size(); ++i)
			systemSizes += systemTypes[i].size;

		size_t offset = 0;
		systemsMemory.resize(systemSizes);
		for (size_t i = 0; i < systems.size(); ++i)
		{
			systems[i] = (BaseSystem*)(systemsMemory.data() + offset);
			systemTypes[i].construct((BaseSystem*)systems[i]);
			offset += systemTypes[i].size;

			systems[i]->ecs = this;
		}
	}
	ECS::~ECS() 
	{
		for (size_t i = 0; i < components.size(); ++i)		
			for (size_t j = 0; j < components[i].size(); j += componentTypes[i].size)			
				componentTypes[i].destruct((BaseComponent*)(components[i].data() + j));			

		for (size_t i = 0; i < systems.size(); ++i)		
			systemTypes[i].destruct(systems[i]);		
	}

	Entity ECS::CreateEntity(String&& name, const std::initializer_list<ComponentTypeID>& componentTypes)
	{
		EntityID entityID = GetNextEntityID();
		EntityData& entity = entities[entityID];
		entity.components.resize(componentTypes.size());

		for (size_t i = 0; i < componentTypes.size(); ++i)
		{
			BaseComponent* component = AllocateComponent(componentTypes.begin()[i]);
			component->entityID = entityID;

			entity.components[i].typeID = componentTypes.begin()[i];
			entity.components[i].index = (uint8*)component - components[componentTypes.begin()[i]].data();
		}
		return Entity(entityID, this);
	}
	Entity ECS::CreateEntity(String&& name, const std::initializer_list<ComponentTypeID>& componentTypes, const std::initializer_list<const BaseComponent*>& components)
	{
		EntityID entityID = GetNextEntityID();
		EntityData& entity = entities[entityID];
		entity.name = std::move(name);
		entity.components.resize(componentTypes.size());

		for (size_t i = 0; i < componentTypes.size(); ++i)
		{
			BaseComponent* component = AllocateComponent(componentTypes.begin()[i], components.begin()[i]);
			component->entityID = entityID;

			entity.components[i].typeID = componentTypes.begin()[i];
			entity.components[i].index = (uint8*)component - this->components[componentTypes.begin()[i]].data();
		}
		return Entity(entityID, this);
	}

	void ECS::DeleteEntity(const Entity& handle)
	{
		EntityData& entity = entities[handle.id];

		for (auto& c : entity.components)
			FreeComponent(c.typeID, c.index);

		entities.erase(handle.id);		
	}

	void ECS::UpdateSystems()
	{
		for (int i = 0; i < systems.size(); ++i)
			systemTypes[i].update(systems[i]);
	}
	void ECS::UpdateSystem(SystemTypeID id)
	{
		systemTypes[id].update(systems[id]);
	}	
	BaseSystem* ECS::GetSystem(SystemTypeID id)
	{
		return systems[id];
	}
	Entity ECS::GetEntity(const String& name)
	{
		for (const auto& e : entities)
			if (e.second.name == name)
				return Entity(e.first, this);
		return Entity(-1, nullptr);
	}
}