#pragma once
#include "BlazeEngine/DataStructures/Array.h"
#include "BlazeEngine/DataStructures/HybridArray.h"
#include "Component.h"
#include "Entity.h"

namespace Blaze::ECS
{
	class BLAZE_API Scene
	{		
	public:		
		Scene();
		~Scene();

		Result SetRegistry(ComponentTypeRegistry registry);

		void Clear();

		template<typename ... T>
		Entity* Create();
		template<typename ... T>
		Entity* Create(T** ... args);
		Entity* Create(std::initializer_list<const ComponentTypeData*> componentsTypeData);
		Entity* Create(std::initializer_list<const ComponentTypeData*> componentsTypeData, Component** components);
		Entity* Create(std::initializer_list<const ComponentTypeData*> componentsTypeData, Component** components, std::function<void(void*)>* constructors);
		Result Destroy(Entity* entity);

		template<typename T>
		Result UpdateSystem();
		Result UpdateSystem(const ComponentTypeData&);
		
		template<typename T> 
		T::System* GetSystem();
		System* GetSystem(const ComponentTypeData&);		

		const ComponentContainer& GetComponents(const ComponentTypeData&);
		
		inline const ComponentTypeRegistry& GetRegistry() const { return registry; }

	private:
		static constexpr uint entityBucketElementCount = 64;

		ComponentTypeRegistry registry;
		Array<System*> systems;		
		Array<ComponentContainer> containers;

		HybridArray<Entity> entities;

		template<typename T>
		void SetComponent(T arg, uint index, Component** components);
	};

	template<typename T>
	void Scene::SetComponent(T arg, uint index, Component** components)
	{
		if (arg != nullptr)
			*arg = (std::remove_pointer_t<T>)components[index];
	}

	template<typename ... T>
	Entity* Scene::Create()
	{		
		return Create({ registry.GetComponentTypeData<T>()... });
	}

	template<typename ... T>
	Entity* Scene::Create(T** ... args)
	{		
		Component* components[sizeof...(T)];
		Entity* entity = Create({ &registry.GetComponentTypeData<T>()... }, components);

		uint i = 0;
		(SetComponent(args, i++, components), ...);
		return entity;
	}

	template<typename T>
	inline T::System* Scene::GetSystem()
	{
		return (T*)GetSystem(registry.GetComponentTypeData<T>());
	}

	template<typename T>
	inline Result Scene::UpdateSystem()
	{
		return UpdateSystem(registry.GetComponentTypeData<T>());
	}
}