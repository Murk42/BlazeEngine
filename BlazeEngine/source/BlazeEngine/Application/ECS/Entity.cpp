#include "pch.h"
#include "BlazeEngine/Application/ECS/Entity.h"
#include "BlazeEngine/Application/ECS/Scene.h"

#include "EntityCreationData.h"

namespace Blaze::ECS
{		
	/*
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
	*/	

	const ComponentTypeData** FindEntityComponentData(Entity* entity, const ComponentTypeData& typeData)
	{
		auto first = GetEntityComponentsTypeData(entity);
		auto last = first + entity->GetComponentCount();		

		auto ptr = first;
		while (ptr != last)
			if (**ptr == typeData)
				break;
			else
				++ptr;

		if (ptr == last)
			return nullptr;

		return ptr;
	}	
	Component* FindEntityComponent(const Entity* entity, const ComponentTypeData& typeData)
	{
		auto first = GetEntityComponentsTypeData(entity);
		auto last = first + entity->GetComponentCount();

		auto ptr = first;
		while (ptr != last)
			if (**ptr == typeData)
				break;
			else
				++ptr;

		if (ptr == last)
			return nullptr;

		auto components = GetEntityComponents(entity);
		uint index = ptr - first;

		return components[index];
	}
			
	Entity::Entity()
		: scene(nullptr), componentCount(0)
	{
		scene = currentEntityCreationData->scene;
		componentCount = currentEntityCreationData->typesData.Count();
	}
	Entity::~Entity()
	{		
	}

	bool Entity::HasComponent(const ComponentTypeData& typeData) const
	{		
		auto ptr = FindEntityComponent(this, typeData);		
		return ptr != nullptr;
	}
	Component* Entity::GetComponent(const ComponentTypeData& typeData) const
	{
		auto ptr = FindEntityComponent(this, typeData);

		if (ptr == nullptr)
			return nullptr;

		return ptr;		
	}
	Component* Entity::GetComponent(uint index) const
	{
		if (index > componentCount)
		{			
			Debug::Logger::LogError("Blaze Engine", StringParsing::Merge("Component index out of range. Index value was: " , index));
			return nullptr;
		}

		return GetEntityComponents(this)[index];
	}		
	const ComponentTypeData* Entity::GetComponentTypeData(uint index)
	{
		if (index > componentCount)
		{
			Debug::Logger::LogError("Blaze Engine", StringParsing::Merge("Component index out of range. Index value was: ", index));
			return nullptr;
		}

		return GetEntityComponentsTypeData(this)[index];
	}
	const ComponentTypeRegistry* Entity::GetRegistry() const
	{		
		return &scene->GetRegistry();
	}
}