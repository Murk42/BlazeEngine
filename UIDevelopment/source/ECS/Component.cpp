#include "pch.h"
#include "Component.h"
#include <bit>

namespace Blaze::ECS
{
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

		uint index = std::countl_one(header->flags);
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
}