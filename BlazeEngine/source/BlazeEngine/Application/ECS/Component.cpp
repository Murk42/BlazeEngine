#include "pch.h"
#include "BlazeEngine/Application/ECS/Component.h"
#include "BlazeEngine/Application/ECS/Entity.h"
#include "BlazeEngine/Application/ECS/Scene.h"

#include "EntityCreationData.h"

namespace Blaze::ECS
{				
	//extern ComponentTypeData emptyComponenTypeData;

	Component::Component()
		: entity(nullptr), system(nullptr)
	{
		if (currentEntityCreationData->scene != nullptr)
		{
			entity = currentEntityCreationData->currentEntity;
			system = currentEntityCreationData->systems[(*currentEntityCreationData->currentTypeData)->Index()];			
			++currentEntityCreationData->currentTypeData;
		}
	}

	bool Component::GetTypeData(const ComponentTypeData*& typeData) const
	{
		if (system == nullptr)
			return false;

		return system->GetTypeData(typeData);		
	}	

	uintMem ComponentContainer::BucketHeader::MarkNew()
	{
		uintMem index = std::countr_one(flags);
		FlagType mask = FlagType(1) << index;

		flags |= mask;

		return index;		
	}	
	void ComponentContainer::BucketHeader::Unmark(uintMem index)
	{
		FlagType mask = FlagType(1) << index;

		if ((flags & mask) == 0)
			BLAZE_ENGINE_ERROR("Trying to free a element in a HybridArray that wasn't allocated yet");

		flags ^= mask;
	}

	bool ComponentContainer::BucketHeader::IsFull()
	{
		return flags == std::numeric_limits<FlagType>::max();
	}

	bool ComponentContainer::BucketHeader::IsEmpty()
	{
		return flags == 0;
	}

	void ComponentContainer::GetComponentLocation(Component* component, BucketHeader*& bucket, uintMem& index)
	{
		void* rawComponent = (byte*)component - typeData->BaseOffset();
		ElementHeader* element = (ElementHeader*)((byte*)rawComponent - sizeof(ElementHeader));
		bucket = element->bucket;

		uintMem byteOffset = (byte*)element - (byte*)bucket - sizeof(BucketHeader);

		if (byteOffset % elementSize != 0)
			BLAZE_ENGINE_ERROR("Invalid component pointer");

		index = byteOffset / elementSize;
	}
	Component* ComponentContainer::GetComponentFromLocation(BucketHeader* bucket, uintMem index, ElementHeader*& element)
	{
		element = (ElementHeader*)((byte*)bucket + sizeof(BucketHeader) + index * elementSize);
		void* rawComponent = (byte*)element + sizeof(ElementHeader);
		return (Component*)((byte*)rawComponent + typeData->BaseOffset());
	}
	void* ComponentContainer::FirstInBucket(BucketHeader* bucket) const
	{
		return (byte*)bucket + sizeof(BucketHeader) + std::countr_zero(bucket->flags) * elementSize + sizeof(ElementHeader);
	}
	void* ComponentContainer::LastInBucket(BucketHeader* bucket) const
	{
		return (byte*)bucket + sizeof(BucketHeader) + (BucketElementCount - 1 - std::countl_zero(bucket->flags)) * elementSize + sizeof(ElementHeader);
	}

	void ComponentContainer::Increment(uintMem& bucketIndex, Component*& component) const
	{
		void* ptr = (byte*)component - typeData->BaseOffset();
		ElementHeader* element = (ElementHeader*)((byte*)component - sizeof(ElementHeader));
		uintMem index = ((byte*)ptr - (byte*)buckets[bucketIndex] - sizeof(BucketHeader)) / elementSize;

		FlagType mask = ~((((FlagType)1) << (index + 1)) - 1);
		index = std::countr_zero(buckets[bucketIndex]->flags & mask);
		if ((buckets[bucketIndex]->flags & mask) == std::numeric_limits<FlagType>::max())
			index = BucketElementCount;

		if (index == BucketElementCount)
		{
			++bucketIndex;

			if (bucketIndex == bucketCount)
				component = nullptr;
			else
				component = (Component*)((byte*)FirstInBucket(buckets[bucketIndex]) + typeData->BaseOffset());
		}
		else
			component = (Component*)((byte*)buckets[bucketIndex] + sizeof(BucketHeader) + index * elementSize + sizeof(ElementHeader) + typeData->BaseOffset());
	}
	void ComponentContainer::Decrement(uintMem& bucketIndex, Component*& component) const
	{
		void* ptr = (byte*)component - typeData->BaseOffset();
		ElementHeader* element = (ElementHeader*)((byte*)component - sizeof(ElementHeader));
		uintMem index = ((byte*)ptr - (byte*)buckets[bucketIndex] - sizeof(BucketHeader)) / elementSize;

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
				component = (Component*)((byte*)LastInBucket(buckets[bucketIndex]) + typeData->BaseOffset());
		}
		else			
			component = (Component*)((byte*)buckets[bucketIndex] + sizeof(BucketHeader) + index * elementSize + sizeof(ElementHeader) + typeData->BaseOffset());
	}

	ComponentContainer::BucketHeader* ComponentContainer::AllocateBucket()
	{
		return (BucketHeader*)Memory::Allocate(sizeof(BucketHeader) + elementSize * BucketElementCount);
	}

	void ComponentContainer::FreeBucket(BucketHeader* bucket)
	{
		Memory::Free(bucket);
	}


	void ComponentContainer::RemoveBucket(BucketHeader* bucket)
	{
		uintMem i = 0;
		for (; i < bucketCount; ++i)
			if (buckets[i] == bucket)
				break;

		if (i == bucketCount)
			BLAZE_ENGINE_ERROR("Trying to free a invalid pointer");

		--bucketCount;
		--nonFullBucketCount;

		buckets[i] = buckets[nonFullBucketCount];
		buckets[nonFullBucketCount] = buckets[bucketCount];

		BucketHeader** newBuckets = new BucketHeader * [bucketCount];
		memcpy(newBuckets, buckets, sizeof(BucketHeader*) * bucketCount);
		delete[] buckets;
		buckets = newBuckets;
	}
	void ComponentContainer::AddBucket(BucketHeader* bucket)
	{
		BucketHeader** newBuckets = new BucketHeader * [bucketCount + 1];
		memcpy(newBuckets + 1, buckets, sizeof(BucketHeader*) * bucketCount);
		delete[] buckets;
		buckets = newBuckets;

		buckets[0] = bucket;
		buckets[0]->flags = 0;

		bucketCount++;
		nonFullBucketCount++;
	}

	ComponentContainer::ComponentContainer()
		: buckets(nullptr), elementCount(0), bucketCount(0), nonFullBucketCount(0), elementSize(0), typeData(nullptr)
	{

	}
	ComponentContainer::~ComponentContainer()
	{
		Clear();
	}
	void ComponentContainer::SetTypeData(const ComponentTypeData& typeData)
	{
		if (typeData.IsNone())
		{
			BLAZE_ENGINE_CORE_ERROR("Trying to set type data to none");
			return;
		}

		this->typeData = &typeData;
		elementSize = typeData.Size() + sizeof(ElementHeader);		
	}
	Component* ComponentContainer::Create()
	{
		Component* component = Allocate();

		typeData->Construct(component);

		return component;
	}
	Component* ComponentContainer::Allocate()
	{
		BucketHeader* bucket;
		if (nonFullBucketCount == 0)
		{
			bucket = AllocateBucket();
			AddBucket(bucket);
		}
		else
			bucket = buckets[0];

		uintMem index = bucket->MarkNew();

		if (bucket->IsFull())
		{
			--nonFullBucketCount;
			std::swap(buckets[0], buckets[nonFullBucketCount]);
		}
		elementCount++;

		ElementHeader* element;
		Component* component = GetComponentFromLocation(bucket, index, element);
		element->bucket = bucket;

		return component;
	}
	void ComponentContainer::Destroy(Component* component)
	{		
		typeData->Destruct(component);

		Free(component);		
	}
	void ComponentContainer::Free(Component* component)
	{
		uintMem index;
		BucketHeader* bucket;

		GetComponentLocation(component, bucket, index);

		bucket->Unmark(index);		
		
		--elementCount;		

		if (bucket->IsEmpty())
		{
			RemoveBucket(bucket);
			FreeBucket(bucket);
		}
	}
	void ComponentContainer::Clear()
	{
		for (auto el : *this)		
			typeData->Destruct(el);

		for (uintMem i = 0; i < bucketCount; ++i)
			FreeBucket(buckets[i]);
				
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
		return Iterator(this, 0, (Component*)((byte*)FirstInBucket(buckets[0]) + typeData->BaseOffset()));
	}
	ComponentContainer::Iterator ComponentContainer::end() const
	{
		return Iterator(this, bucketCount, nullptr);
	}

	ComponentContainer::Iterator::Iterator(const ComponentContainer* cc, uintMem bucketIndex, Component* ptr)
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

	//ComponentGroup::ComponentGroup()
	//	: data(nullptr), stateSize(0), count(0)
	//{
	//}
	//ComponentGroup::~ComponentGroup()
	//{
	//	Clear();
	//}
	//
	//Result ComponentGroup::SetupTypes(std::initializer_list<ComponentTypeData*> componentsTypeData, const ComponentTypeRegistry* registry)
	//{
	//	this->count = componentsTypeData.size();
	//	this->stateSize = ((registry->GetAllTypesData().size() + 7) >> 3);
	//
	//	size_t size = stateSize + sizeof(Component*) * count;
	//
	//	data = Memory::Allocate(size);
	//
	//	memset(data, 0, size);
	//	for (auto typeIndex : typeIndicies)
	//	{
	//		if (typeIndex >= typeCount)
	//		{
	//			Clear();
	//			return BLAZE_ENGINE_CORE_ERROR("Invalid type index: " + StringParsing::Convert(typeIndex).value);
	//		}
	//
	//		size_t stateOffset = typeIndex >> 3;
	//		char stateMask = 1 << (typeIndex & 7);
	//		*((char*)data + stateOffset) |= stateMask;
	//	}
	//
	//	return Result();
	//}
	//void ComponentGroup::Clear()
	//{
	//	Memory::Free(data);
	//	data = nullptr;
	//	stateSize = 0;
	//}
	//
	//Result ComponentGroup::SetComponent(Component* component)
	//{
	//	if (component == nullptr)
	//		return BLAZE_ENGINE_CORE_ERROR("Component is nullptr");
	//
	//	if (data == nullptr)
	//		return BLAZE_ENGINE_CORE_ERROR("ComponentGroup is empty, cant set components");
	//
	//	auto typeIndex = component->GetTypeData()->Index();
	//	size_t index = CountOnes((byte*)data, typeIndex);
	//
	//	((Component**)((char*)data + stateSize))[index] = component;
	//
	//	return Result();
	//}
	//
	//size_t ComponentGroup::Count() const
	//{
	//	return count;
	//}
	//
	//bool ComponentGroup::HasComponent(uint typeIndex) const
	//{
	//	size_t stateOffset = typeIndex >> 3;
	//	unsigned char stateMask = 1 << (typeIndex & 7);
	//
	//	return (bool)(*((char*)data + stateOffset) & stateMask);
	//}
	//Component* ComponentGroup::GetComponent(uint typeIndex) const
	//{
	//	if (HasComponent(typeIndex))
	//	{
	//		size_t index = CountOnes((byte*)data, typeIndex);
	//		return ((Component**)((char*)data + stateSize))[index];
	//	}
	//	else
	//		return nullptr;
	//}
	//
	//Component** ComponentGroup::begin() const
	//{
	//	return ((Component**)((char*)data + stateSize));
	//}
	//Component** ComponentGroup::end() const
	//{
	//	return ((Component**)((char*)data + stateSize)) + count;
	//}
	//const ComponentTypeRegistry& Component::GetRegistry() const
	//{
	//	return entity->GetScene()->GetManager()->GetRegistry();
	//}
}