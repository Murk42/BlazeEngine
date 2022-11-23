#pragma once
#include "Manager.h"
#include "Component.h"
#include "Entity.h"

namespace Blaze::ECS
{
	class Scene
	{
		static constexpr uint entityBucketElementCount = 64;

		const Manager* manager;
		Array<ComponentContainer> containers;

		struct EntityBucket
		{
			uint16 flags;
			byte data[];
		};
		std::vector<EntityBucket*> entityBuckets;
		uint entityCount;
		template<typename T>
		void SetComponent(T*& arg, uint index, Component** components)
		{
			arg = (T*)components[index];
		}
	public:
		Scene();
		~Scene();

		Result SetManager(const Manager* manager);

		void Clear();

		template<typename ... T>
		Entity* Create()
		{
			auto& registry = manager->GetRegistry();
			return Create({ registry.GetComponentTypeIndex<T>()... });
		}	
		template<typename ... T>
		Entity* Create(T*& ... args)
		{
			auto& registry = manager->GetRegistry();
			Component* components[sizeof...(T)];
			Entity* entity = Create({ registry.GetComponentTypeIndex<T>()... }, components);

			uint i = 0;
			(SetComponent(args, i++, components), ...);
			return entity;
		}
		Entity* Create(std::initializer_list<uint> componentTypeIndexes);
		Entity* Create(std::initializer_list<uint> componentTypeIndexes, Component** components);
		Result Destroy(Entity* entity);

		inline const Manager* GetManager() const { return manager; }
	};
}