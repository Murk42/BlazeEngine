#pragma once
#include "Component.h"
#include "ComponentTypeRegistry.h"
#include <span>

namespace Blaze::ECS
{
	class Scene;

	class BLAZE_API Entity
	{
		Scene* scene;				

		size_t componentCount;
		Component** components;
		void* data;

		void SetupComponents(std::initializer_list<const ComponentTypeData*> components);
		void SetComponent(uint index, Component* component);

		const ComponentTypeRegistry* GetRegistry() const;
	public:				
		Entity();
		~Entity();

		template<typename T>
		bool HasComponent() const;
		bool HasComponent(const ComponentTypeData&) const;

		template<typename Component>
		Component* GetComponent() const;
		Component* GetComponent(const ComponentTypeData&) const;
				
		std::span<Component*, std::dynamic_extent> GetComponents() const;

		Scene* GetScene() const { return scene; }

		friend class Scene;
	};

	template<typename T>
	bool Entity::HasComponent() const
	{
		return HasComponent(GetRegistry()->GetComponentTypeData<T>());
	}

	template<typename T>
	T* Entity::GetComponent() const
	{
		return GetComponent(GetRegistry()->GetComponentTypeData<T>())->Cast<T>();
	}
}