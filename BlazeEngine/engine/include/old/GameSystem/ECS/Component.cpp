#include "Component.h"
#include "ECS.h"
#include "BlazeEngine/Internal/TypeStructs.h"

namespace Blaze
{	
	extern std::vector<ComponentType> componentTypes;		
	
	BaseComponent::BaseComponent(ComponentTypeID typeID)
		: entityID(-1), ecs(nullptr), typeID(typeID)
	{
	}

	BaseComponent::BaseComponent(const BaseComponent& c)
		: ecs(nullptr), entityID(-1), typeID(c.typeID)
	{

	}

	ComponentTypeID BaseComponent::RegisterComponentType(Func<> construct, Func<> destruct, size_t size, size_t offset, size_t hash, Func<const void*> copy, std::initializer_list<std::pair<size_t, size_t>> bases)
	{
		componentTypes.push_back({ construct, destruct, size, offset, hash, copy, bases});
		return componentTypes.size() - 1;		
	}
	const void* BaseComponent::GetBase(size_t hash) const
	{
		const auto& type = componentTypes[typeID];
		auto it = std::find_if(type.bases.begin(), type.bases.end(), [&](const std::pair<size_t, size_t>& p) { return p.second == hash; });

		if (it != type.bases.end())		
			return (const void*)((const uint8*)type.ToDerived(this) + it->second);		

		return nullptr;
	}
	void* BaseComponent::GetBase(size_t hash)
	{
		const auto& type = componentTypes[typeID];
		auto it = std::find_if(type.bases.begin(), type.bases.end(), [&](const std::pair<size_t, size_t>& p) { return p.first == hash; });

		if (it != type.bases.end())
			return (void*)((uint8*)type.ToDerived(this) + it->second);

		return nullptr;
	}
}