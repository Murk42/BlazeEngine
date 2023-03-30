#include "BlazeEngine/Application/ECS/Component.h"
#include "BlazeEngine/Application/ECS/Entity.h"
#include "BlazeEngine/Application/ECS/Scene.h"
#include <bit>

namespace Blaze::ECS
{	
	ComponentContainer::ElementHeader* ComponentContainer::BucketAllocate(BucketHeader* bucket)
	{
		uint index = std::countr_one(bucket->flags);
		FlagType mask = FlagType(1) << index;

		bucket->flags |= mask;

		ElementHeader* element = (ElementHeader*)((byte*)bucket + sizeof(BucketHeader) + index * elementSize);
		element->bucket = bucket;

		typeData.construct((byte*)element + sizeof(ElementHeader));		
		return element;
	}
	void ComponentContainer::BucketFree(ElementHeader* ptr)
	{
		BucketHeader* bucket = ptr->bucket;
		uint index = ((byte*)ptr - (byte*)bucket - sizeof(BucketHeader)) / elementSize;
		FlagType mask = FlagType(1) << index;

		if ((bucket->flags & mask) == 0)
			Logger::AddLog(BLAZE_FATAL_LOG("Blaze Engine", "Trying to free a element in a HybridArray that wasn't allocated yet"));

		bucket->flags ^= mask;		
		typeData.destruct((byte*)ptr + sizeof(ElementHeader));
	}
	void* ComponentContainer::FirstInBucket(BucketHeader* bucket) const
	{
		return (byte*)bucket + sizeof(BucketHeader) + std::countr_zero(bucket->flags) * elementSize + sizeof(ElementHeader);
	}
	void* ComponentContainer::LastInBucket(BucketHeader* bucket) const
	{
		return (byte*)bucket + sizeof(BucketHeader) + (BucketElementCount - 1 - std::countl_zero(bucket->flags)) * elementSize + sizeof(ElementHeader);
	}

	void ComponentContainer::Increment(uint& bucketIndex, Component*& component) const
	{
		void* ptr = (byte*)component - typeData.baseOffset;
		ElementHeader* element = (ElementHeader*)((byte*)component - sizeof(ElementHeader));
		uint index = ((byte*)ptr - (byte*)buckets[bucketIndex] - sizeof(BucketHeader)) / elementSize;

		FlagType mask = ~((((FlagType)1) << (index + 1)) - 1);
		index = std::countr_zero(buckets[bucketIndex]->flags & mask);

		if (index == BucketElementCount)
		{
			++bucketIndex;

			if (bucketIndex == bucketCount)
				component = nullptr;
			else
				component = (Component*)((byte*)FirstInBucket(buckets[bucketIndex]) + typeData.baseOffset);
		}
		else
			component = (Component*)((byte*)buckets[bucketIndex] + sizeof(BucketHeader) + index * elementSize + sizeof(ElementHeader) + typeData.baseOffset);
	}
	void ComponentContainer::Decrement(uint& bucketIndex, Component*& component) const
	{
		void* ptr = (byte*)component - typeData.baseOffset;
		ElementHeader* element = (ElementHeader*)((byte*)component - sizeof(ElementHeader));
		uint index = ((byte*)ptr - (byte*)buckets[bucketIndex] - sizeof(BucketHeader)) / elementSize;

		FlagType mask = (((FlagType)1) << index) - 1;
		index = std::countl_zero(buckets[bucketIndex]->flags & mask);

		if (index > BucketElementCount)
		{
			--bucketIndex;

			if (bucketIndex > bucketCount)
			{
				bucketIndex = bucketCount;
				component = nullptr;
			}
			else
				component = (Component*)((byte*)LastInBucket(buckets[bucketIndex]) + typeData.baseOffset);
		}
		else			
			component = (Component*)((byte*)buckets[bucketIndex] + sizeof(BucketHeader) + index * elementSize + sizeof(ElementHeader) + typeData.baseOffset);
	}

	ComponentContainer::ComponentContainer()
		: buckets(nullptr), elementCount(0), bucketCount(0), nonFullBucketCount(0)
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
		elementSize = typeData.size + sizeof(ElementHeader);
		return Result();
	}
	Component* ComponentContainer::Create()
	{
		if (nonFullBucketCount == 0)
		{
			BucketHeader** newBuckets = new BucketHeader*[bucketCount + 1];
			memcpy(newBuckets + 1, buckets, sizeof(BucketHeader*) * bucketCount);
			delete[] buckets;
			buckets = newBuckets;

			buckets[0] = (BucketHeader*)Memory::Allocate(sizeof(BucketHeader) + elementSize * BucketElementCount);
			buckets[0]->flags = 0;

			bucketCount++;
			nonFullBucketCount++;
		}

		elementCount++;
		ElementHeader* element = BucketAllocate(buckets[0]);

		if (buckets[0]->flags == std::numeric_limits<FlagType>::max())
		{
			--nonFullBucketCount;
			std::swap(buckets[0], buckets[nonFullBucketCount]);
		}

		return (Component*)((byte*)element + sizeof(ElementHeader) + typeData.baseOffset);
	}
	void ComponentContainer::Destroy(Component* component)
	{
		void* ptr = (byte*)component - typeData.baseOffset;
		ElementHeader* element = (ElementHeader*)((byte*)component - sizeof(ElementHeader));		
		BucketHeader* bucket = element->bucket;
		BucketFree(element);

		if (bucket->flags == 0)
		{
			uint i = 0;
			for (; i < bucketCount; ++i)
				if (buckets[i] == bucket)
					break;

			if (i == bucketCount)
				Logger::AddLog(BLAZE_FATAL_LOG("Blaze Engine", "Trying to free a invalid pointer"));

			--bucketCount;
			--nonFullBucketCount;

			Memory::Free(buckets[i]);
			buckets[i] = buckets[nonFullBucketCount];
			buckets[nonFullBucketCount] = buckets[bucketCount];

			BucketHeader** newBuckets = new BucketHeader * [bucketCount];
			memcpy(newBuckets, buckets, sizeof(BucketHeader*) * bucketCount);
			delete[] buckets;
			buckets = newBuckets;
		}

		--elementCount;
	}
	void ComponentContainer::Clear()
	{
		for (uint i = 0; i < bucketCount; ++i)
		{				
			uint index = std::countr_zero(buckets[i]->flags);
			ElementHeader* first = (ElementHeader*)((byte*)buckets[i] + sizeof(BucketHeader) + index * elementSize);
			
			while (true)
			{
				FlagType mask = ~((((FlagType)1) << (index + 1)) - 1);
				index = std::countr_zero(buckets[i]->flags & mask);			

				if (index == BucketElementCount)
					break;

				typeData.destruct((byte*)first + index * elementSize + sizeof(ElementHeader));				

			}			

			Memory::Free(buckets[i]);
		}
		delete[] buckets;

		elementCount = 0;
		bucketCount = 0;
		buckets = nullptr;
		nonFullBucketCount = 0;
	}
	ComponentContainer::Iterator ComponentContainer::begin() const
	{
		if (bucketCount == 0)
			return end();
		return Iterator(this, 0, (Component*)((byte*)FirstInBucket(buckets[0]) + typeData.baseOffset));
	}
	ComponentContainer::Iterator ComponentContainer::end() const
	{
		return Iterator(this, bucketCount, nullptr);
	}

	ComponentContainer::Iterator::Iterator(const ComponentContainer* cc, uint bucketIndex, Component* ptr)
		: container(cc), bucketIndex(bucketIndex), ptr(ptr)
	{

	}
	ComponentContainer::Iterator::Iterator()
		: container(nullptr), bucketIndex(0), ptr(nullptr)
	{

	}
	ComponentContainer::Iterator::Iterator(const Iterator& o)
		: container(o.container), bucketIndex(o.bucketIndex), ptr(o.ptr)
	{

	}

	ComponentContainer::Iterator& ComponentContainer::Iterator::operator++()
	{
		container->Increment(bucketIndex, ptr);
		return *this;
	}
	ComponentContainer::Iterator ComponentContainer::Iterator::operator++(int)
	{
		Iterator out = *this;
		++* this;
		return out;
	}
	ComponentContainer::Iterator& ComponentContainer::Iterator::operator--()
	{
		container->Decrement(bucketIndex, ptr);
		return *this;
	}
	ComponentContainer::Iterator ComponentContainer::Iterator::operator--(int)
	{
		Iterator out = *this;
		--* this;
		return out;
	}



	bool ComponentContainer::Iterator::operator==(const Iterator& other) const
	{
		return container == other.container && ptr == other.ptr;
	}
	bool ComponentContainer::Iterator::operator!=(const Iterator& other) const
	{
		return container != other.container || ptr != other.ptr;
	}

	Component* ComponentContainer::Iterator::operator*()
	{
		return ptr;
	}

	Component* ComponentContainer::Iterator::operator->()
	{
		if (ptr == nullptr)
			throw;
		return ptr;
	}

	ComponentContainer::Iterator& ComponentContainer::Iterator::operator=(const Iterator& other)
	{
		container = other.container;
		bucketIndex = other.bucketIndex;
		ptr = other.ptr;
		return *this;
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
				return BLAZE_ERROR_RESULT("Blaze Engine", "Invalid type index: " + StringParsing::Convert(typeIndex).value);
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