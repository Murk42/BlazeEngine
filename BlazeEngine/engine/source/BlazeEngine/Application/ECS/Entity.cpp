#include "BlazeEngine/Application/ECS/Entity.h"
#include "BlazeEngine/Application/ECS/Scene.h"

namespace Blaze::ECS
{	
	static inline size_t CountOnesUntil(uint8* buffer, size_t size, size_t endBitIndex)
	{
		if (endBitIndex == 0)
			return 0;

		if (endBitIndex > size * 8)
			throw;

		size_t count = 0;
		uint _count;				

		while (endBitIndex > 64)
		{			
			count += std::popcount<uint64>(*(uint64*)buffer);			
			buffer += 8;
			endBitIndex -= 64;
		}

		if (endBitIndex >= 32)
		{
			count += std::popcount<uint32>(*(uint32*)buffer);			
			buffer += 4;
			endBitIndex -= 32;
		}

		if (endBitIndex >= 16)
		{
			count += std::popcount<uint16>(*(uint16*)buffer);			
			buffer += 2;
			endBitIndex -= 16;
		}

		if (endBitIndex >= 8)
		{
			count += std::popcount<uint8>(*(uint8*)buffer);						
			buffer += 1;
			endBitIndex -= 8;
		}
		
		count += std::popcount<uint8>((*buffer & (uint8)((1 << endBitIndex) - 1)));
		return count;
	}

	static inline void FlipBit(uint8* buffer, uint index)
	{
		uint8 mask = 1i8 << (index % 8);
		uint offset = index / 8;
		uint8& byte = *(buffer + offset);
		byte ^= mask;
	}

	static inline bool GetBit(uint8* buffer, uint index)
	{
		uint8 mask = 1i8 << (index % 8);
		uint offset = index / 8;
		uint8& byte = *(buffer + offset);

		return (bool)(byte & mask);
	}

	void Entity::SetupComponents(std::initializer_list<const ComponentTypeData*> componentsTypeData)
	{
		auto allComponentsTypeData = scene->GetRegistry().GetAllTypesData();
		auto typeCount = allComponentsTypeData.size();
		componentCount = componentsTypeData.size();
		size_t stateSize = (typeCount + 7) / 8;
		data = Memory::Allocate(stateSize + sizeof(Component*) * componentCount);
		components = (Component**)((byte*)data + stateSize);

		memset(data, 0, stateSize);

		for (auto typeData : componentsTypeData)
		{
			FlipBit((uint8*)data, typeData->Index());
		}
	}
	void Entity::SetComponent(uint index, Component* component)
	{
		components[index] = component;
	}
	Entity::Entity()
		: scene(nullptr), componentCount(0), components(nullptr), data(nullptr)
	{
	}
	Entity::~Entity()
	{
		Memory::Free(data);
	}
	const ComponentTypeRegistry* Entity::GetRegistry() const
	{
		return &scene->GetRegistry();
	}
	bool Entity::HasComponent(const ComponentTypeData& typeData) const
	{
		return GetBit((uint8*)data, typeData.Index());		
	}
	Component* Entity::GetComponent(const ComponentTypeData& typeData) const
	{
		if (!HasComponent(typeData))
			return nullptr;

		uint index = CountOnesUntil((uint8*)data, (uint8*)components - (uint8*)data, typeData.Index());
		return components[index];
	}
	std::span<Component*, std::dynamic_extent> Entity::GetComponents() const
	{
		return std::span<Component*, std::dynamic_extent>(components, components + componentCount);
	}
}