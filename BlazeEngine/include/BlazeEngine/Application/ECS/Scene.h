#pragma once
#include "BlazeEngine/Application/ECS/ComponentTypeRegistry.h"
#include "BlazeEngine/Application/ECS/EntityView.h"

namespace Blaze::ECS
{	
	class BLAZE_API Scene
	{		
	public:		
		Scene();
		~Scene();

		void SetRegistry(ComponentTypeRegistry registry);

		void Clear();		

		Entity* Create(ArrayView<const ComponentTypeData*> typesData);		
		template<typename ... C> requires (IsComponent<C> && ...)
		EntityView<C...> Create();		

		void Destroy(Entity* entity);

		template<typename C> requires IsComponent<C>
		void UpdateSystem();
		void UpdateSystem(const ComponentTypeData&);
		void UpdateSystem(uintMem index);
		
		template<typename C> requires IsComponent<C>
		typename C::template System* GetSystem();
		System* GetSystem(const ComponentTypeData&);
		System* GetSystem(uintMem index);

		template<typename C> requires IsComponent<C>
		const ComponentContainer& GetComponents();
		const ComponentContainer& GetComponents(const ComponentTypeData&);
		const ComponentContainer& GetComponents(uintMem index);

		ArrayView<Entity*> GetEntities() const;
		
		inline const ComponentTypeRegistry& GetRegistry() const { return registry; }

		friend class PredefinedEntity;
		template<typename ... Cs> requires (IsComponent<Cs> && ...)
		friend class CustomEntity;
	private:
		static constexpr uintMem entityBucketElementCount = 64;

		ComponentTypeRegistry registry;
		Array<System*> systems;		
		Array<ComponentContainer> containers;
		Array<Entity*> entities;
		
		Entity* CreateEntity(ArrayView<const ComponentTypeData*> typesData);
		ECS::Component** AllocateComponents();
		Component* GetCurrentComponent();
		void FinishEntityCreation();		

		template<typename C> requires IsComponent<C>
		inline void SetTypeData(const ComponentTypeData*& ptr);
		template<typename ... C> requires (IsComponent<C> && ...)
		inline void SetTypesData(const ComponentTypeData* (&ptr)[sizeof...(C)]);
	};	
	
	
	template<typename ... C> requires (IsComponent<C> && ...)
		inline EntityView<C...> Scene::Create()
	{
		const ComponentTypeData* typesData[sizeof...(C)];

		SetTypesData<C...>(typesData);

		auto entity = CreateEntity(typesData);
		Component** rawComponents = AllocateComponents();

		for (auto typeData : typesData)
			typeData->Construct(GetCurrentComponent());

		FinishEntityCreation();

		uint i = 0;
		Tuple<C*...> components{
			 (C*)rawComponents[i++]...
		};

		return EntityView<C...>(entity, components);
	}

	template<typename C> requires IsComponent<C>
	inline void Scene::UpdateSystem()
	{
		const ComponentTypeData* typeData;

		if (!registry.GetComponentTypeData<C>(typeData))
		{
			BLAZE_ENGINE_ERROR("Component type is not in registry");
			return;
		}

		return UpdateSystem(*typeData);
	}

	template<typename C> requires IsComponent<C>
	inline typename C::template System* Scene::GetSystem()
	{
		const ComponentTypeData* typeData;

		if (!registry.GetComponentTypeData<C>(typeData))
			BLAZE_ENGINE_CORE_ERROR("Component type is not in registry");
		
		return (typename C::template System*)GetSystem(*typeData);
	}

	template<typename C> requires IsComponent<C>
	inline const ComponentContainer& Scene::GetComponents()
	{
		const ComponentTypeData* typeData;

		if (!registry.GetComponentTypeData<C>(typeData))
			BLAZE_ENGINE_CORE_ERROR("Component type is not in registry");

		return GetComponents(*typeData);
	}	

	template<typename C> requires IsComponent<C>
	inline void Scene::SetTypeData(const ComponentTypeData*& ptr)
	{
		if (!registry.GetComponentTypeData<C>(ptr))
			BLAZE_ENGINE_CORE_ERROR("Registry doesnt contain the component type");
	}

	template<typename ... C> requires (IsComponent<C> && ...)
	inline void Scene::SetTypesData(const ComponentTypeData* (&ptr)[sizeof...(C)])
	{
		uintMem i = 0;
		(SetTypeData<C>(ptr[i++]), ...);
	}
}