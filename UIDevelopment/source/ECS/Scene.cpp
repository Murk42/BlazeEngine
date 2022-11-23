#include "Scene.h"

namespace Blaze::ECS
{
	Scene::Scene()
		: entityCount(0)
	{
	}
	Scene::~Scene()
	{
		Clear();
	}
	Result Scene::SetManager(const Manager* manager)
	{
		this->manager = manager;
		auto& registry = manager->GetRegistry();
		size_t typeCount = registry.GetComponentTypeCount();
		containers.Resize(typeCount);

		for (int i = 0; i < typeCount; ++i)
			containers[i].SetTypeData(registry.GetComponentTypeData(i));

		return Result();
	}
	void Scene::Clear()
	{
		for (auto bucket : entityBuckets)
		{			
			for (size_t offset = 0; offset != entityCount * sizeof(Entity); offset += sizeof(Entity))
				((Entity*)(bucket->data + offset))->~Entity();

			Memory::Free(bucket);
		}

		entityCount = 0;
	}
	Entity* Scene::Create(std::initializer_list<uint> componentTypeIndexes)
	{		
		return Create(componentTypeIndexes, nullptr);
	}
	Entity* Scene::Create(std::initializer_list<uint> componentTypeIndexes, Component** components)
	{
		EntityBucket* bucket = nullptr;
		uint bucketIndex = 0;
		if (entityBuckets.size() * entityBucketElementCount == entityCount)
		{
			bucket = (EntityBucket*)Memory::Allocate(sizeof(EntityBucket) + sizeof(Entity) * entityBucketElementCount);
			bucketIndex = entityBuckets.size();
			bucket->flags = 0;

			entityBuckets.emplace_back(bucket);
		}
		else for (auto it : entityBuckets)
			if (it->flags == std::numeric_limits<decltype(it->flags)>::max())
			{
				bucket = it;
				break;
			}
			else
				++bucketIndex;

		uint index = std::countl_one(bucket->flags);
		Entity* ptr = (Entity*)(bucket->data + sizeof(Entity) * index);
		uint16 mask = 1 << index;

		ptr->bucketIndex = bucketIndex;

		new (ptr) Entity();

		bucket->flags ^= mask;

		ptr->components.Resize(componentTypeIndexes.size());


		uint i = 0;
		for (auto& typeIndex : componentTypeIndexes)
		{
			Component* component = containers[typeIndex].Create();
			component->typeIndex = typeIndex;

			ptr->components[i] = component;

			if (components != nullptr)
				components[i] = component;
			++i;
		}

		++entityCount;
		return ptr;
	}
	Result Scene::Destroy(Entity* entity)
	{
		if (entity == nullptr)
			return Result();
		if (entity->bucketIndex >= entityBuckets.size())
			return BLAZE_ERROR_RESULT("Blaze Engine", "Destroying a entity that doesnt belong in this scene");		

		uint bucketIndex = entity->bucketIndex;
		EntityBucket* header = entityBuckets[bucketIndex];

		uint index = ((byte*)entity - header->data) / entityBucketElementCount;
		uint16 mask = 1 << index;

		if (index >= entityBucketElementCount)
			return BLAZE_ERROR_RESULT("Blaze Engine", "Destroying a entity that doesnt belong in this scene");

		bool flag = header->flags & mask;

		if (!flag)
			return BLAZE_ERROR_RESULT("Blaze Engine", "Destroying a entity that was already destroyed");

		for (auto component : entity->components)
			containers[component->typeIndex].Destroy(component);
		entity->~Entity();

		header->flags ^= mask;

		if (header->flags == 0)
		{
			Memory::Free(header);
			entityBuckets.erase(entityBuckets.begin() + bucketIndex);
		}

		--entityCount;
		return Result();
	}
}