#include "BlazeEngine/Application/ECS/Component.h"
#include "BlazeEngine/Application/ECS/Entity.h"
#include "BlazeEngine/Application/ECS/Scene.h"
#include "BlazeEngine/Memory/MemoryManager.h"
#include <bit>

namespace Blaze::ECS
{
	ComponentContainer::Iterator::Iterator(const ComponentContainer* cc, uint bucketIndex, uint index)
		: container(cc), bucketIndex(bucketIndex), index(index)
	{

	}

	ComponentContainer::Iterator::Iterator(const Iterator& o)
		: container(o.container), bucketIndex(o.bucketIndex), index(o.index)
	{

	}

	ComponentContainer::Iterator ComponentContainer::Iterator::operator++()
	{
		auto old = *this;
		++index;
		if (index == ComponentContainer::bucketElementCount)
		{
			index = 0;
			++bucketIndex;
		}
		return old;
	}
	ComponentContainer::Iterator& ComponentContainer::Iterator::operator++(int)
	{		
		++index;
		if (index >= ComponentContainer::bucketElementCount)
		{
			index = 0;
			++bucketIndex;
		}
		return *this;
	}
	ComponentContainer::Iterator ComponentContainer::Iterator::operator--()
	{
		auto old = *this;
		--index;
		if (index >= ComponentContainer::bucketElementCount)
		{
			index = ComponentContainer::bucketElementCount - 1;
			--bucketIndex;
		}
		return old;
	}
	ComponentContainer::Iterator& ComponentContainer::Iterator::operator--(int)
	{		
		--index;
		if (index >= ComponentContainer::bucketElementCount)
		{
			index = ComponentContainer::bucketElementCount - 1;
			--bucketIndex;
		}
		return *this;
	}

	bool ComponentContainer::Iterator::operator==(const Iterator& other) const
	{
		return container == other.container && bucketIndex == other.bucketIndex && index == other.index;
	}
	bool ComponentContainer::Iterator::operator!=(const Iterator& other) const
	{
		return container != other.container || bucketIndex != other.bucketIndex || index != other.index;
	}

	Component* ComponentContainer::Iterator::operator*()
	{
		return (Component*)(container->buckets[bucketIndex]->data + container->typeData.size * index);
	}
	Component* ComponentContainer::Iterator::operator->()
	{
		return (Component*)(container->buckets[bucketIndex]->data + container->typeData.size * index);
	}

	ComponentContainer::Iterator& ComponentContainer::Iterator::operator=(const Iterator& o)
	{
		container = o.container;
		bucketIndex = o.bucketIndex;
		index = o.index;
		return *this;
	}

	ComponentContainer::ComponentContainer()
		: count(0)
	{

	}
	ComponentContainer::~ComponentContainer()
	{
		Clear();
	}
	Result ComponentContainer::SetTypeData(const ComponentTypeData& typeData)
	{
		if (this->typeData.typeName.Size() != 0)
			return BLAZE_ERROR_RESULT("BlazeEngine", "Cant change component type data");

		this->typeData = typeData;

		return Result();
	}
	void ComponentContainer::Clear()
	{
		for (auto bucket : buckets)
		{
			for (size_t offset = 0; offset != count * typeData.size; offset += typeData.size)
				typeData.destruct(bucket->data + offset);

			Memory::Free(bucket);
		}
		count = 0;
	}
	Component* ComponentContainer::Create()
	{
		BucketHeader* header = nullptr;
		uint bucketIndex = 0;

		if (count == buckets.size() * bucketElementCount)
		{
			header = (BucketHeader*)Memory::Allocate(sizeof(BucketHeader) + typeData.size * bucketElementCount);
			header->flags = 0;

			bucketIndex = buckets.size();
			buckets.emplace_back(header);
		}
		else for (auto bucket : buckets)
			if (bucket->flags != std::numeric_limits<decltype(bucket->flags)>::max())
			{
				header = bucket;
				break;
			}
			else
				++bucketIndex;

		uint index = std::countr_one(header->flags);
		Component* ptr = (Component*)(header->data + typeData.size * index);
		uint16 mask = 1 << index;
		ptr->bucketIndex = bucketIndex;

		typeData.construct(ptr);

		header->flags ^= mask;
		++count;
		return (Component*)ptr;
	}
	Result ComponentContainer::Destroy(Component* component)
	{
		if (component == nullptr)
			return Result();
		if (component->bucketIndex >= buckets.size())
			return BLAZE_ERROR_RESULT("Blaze Engine", "Destroying a component that doesnt belong in this container");

		uint bucketIndex = component->bucketIndex;
		BucketHeader* header = buckets[bucketIndex];

		uint index = ((byte*)component - header->data) / bucketElementCount;
		uint16 mask = 1 << index;

		if (index >= bucketElementCount)
			return BLAZE_ERROR_RESULT("Blaze Engine", "Destroying a component that doesnt belong in this container");

		bool flag = header->flags & mask;

		if (!flag)
			return BLAZE_ERROR_RESULT("Blaze Engine", "Destroying a component that was already destroyed");

		typeData.destruct(component);

		header->flags ^= mask;

		if (header->flags == 0)
		{
			Memory::Free(header);
			buckets.erase(buckets.begin() + bucketIndex);
		}

		--count;
		return Result();
	}
	ComponentContainer::Iterator ComponentContainer::begin() const
	{
		return Iterator(this, 0, 0);
	}
	ComponentContainer::Iterator ComponentContainer::end() const
	{
		if (buckets.size() == 0)
			return begin();

		auto& back = buckets.back();

		if (back->flags == std::numeric_limits<decltype(BucketHeader::flags)>::max())
			return Iterator(this, buckets.size(), 0);

		uint index = std::countr_one(back->flags);

		return Iterator(this, buckets.size() - 1, index);
	}

	size_t CountOnes(byte* buffer, size_t endBitIndex)
	{
		size_t index = 0;

		while (endBitIndex > 64)
		{
			index += std::popcount<uint64>(*(uint64*)buffer);
			buffer += 8;
			endBitIndex -= 64;
		}
		if (endBitIndex > 32)
		{
			index += std::popcount<uint32>(*(uint32*)buffer);
			buffer += 4;
			endBitIndex -= 32;
		}
		if (endBitIndex > 16)
		{
			index += std::popcount<uint16>(*(uint16*)buffer);
			buffer += 2;
			endBitIndex -= 16;
		}
		if (endBitIndex > 8)
		{
			index += std::popcount<uint8>(*(uint8*)buffer);
			buffer += 1;
			endBitIndex -= 8;
		}
	
		uint8 mask = (1 << endBitIndex) - 1;
		index += std::popcount<uint8>((*(uint8*)buffer) & mask);

		return index;
	}

	ComponentGroup::ComponentGroup()
		: data(nullptr), stateSize(0), count(0)
	{
	}
	ComponentGroup::~ComponentGroup()
	{
		Clear();
	}

	Result ComponentGroup::Resize(std::initializer_list<uint> typeIndicies, size_t typeCount)
	{
		this->count = typeIndicies.size();
		this->stateSize = ((typeCount + 7) >> 3);

		size_t size = stateSize + sizeof(Component*) * count;

		data = Memory::Allocate(size);

		memset(data, 0, size);
		for (auto typeIndex : typeIndicies)
		{
			if (typeIndex >= typeCount)
			{
				Clear();
				return BLAZE_ERROR_RESULT("Blaze Engine", "Invalid type index: " + String::Convert(typeIndex));
			}

			size_t stateOffset = typeIndex >> 3;
			char stateMask = 1 << (typeIndex & 7);
			*((char*)data + stateOffset) |= stateMask;
		}

		return Result();
	}
	void ComponentGroup::Clear()
	{
		Memory::Free(data);
		data = nullptr;
		stateSize = 0;
	}

	Result ComponentGroup::SetComponent(Component* component)
	{
		if (component == nullptr)
			return BLAZE_ERROR_RESULT("Blaze Engine", "Component is nullptr");

		if (data == nullptr)
			return BLAZE_ERROR_RESULT("Blaze Engine", "ComponentGroup is empty, cant set components");

		size_t typeIndex = component->GetTypeIndex();
		size_t index = CountOnes((byte*)data, typeIndex);

		((Component**)((char*)data + stateSize))[index] = component;

		return Result();
	}

	size_t ComponentGroup::Count() const
	{
		return count;
	}

	bool ComponentGroup::HasComponent(uint typeIndex) const
	{
		size_t stateOffset = typeIndex >> 3;
		unsigned char stateMask = 1 << (typeIndex & 7);

		return (bool)(*((char*)data + stateOffset) & stateMask);
	}
	Component* ComponentGroup::GetComponent(uint typeIndex) const
	{
		if (HasComponent(typeIndex))
		{
			size_t index = CountOnes((byte*)data, typeIndex);
			return ((Component**)((char*)data + stateSize))[index];
		}
		else
			return nullptr;
	}

	Component** ComponentGroup::begin() const
	{
		return ((Component**)((char*)data + stateSize));
	}
	Component** ComponentGroup::end() const
	{
		return ((Component**)((char*)data + stateSize)) + count;
	}
	const ComponentTypeRegistry& Component::GetRegistry() const
	{
		return entity->GetScene()->GetManager()->GetRegistry();
	}
}