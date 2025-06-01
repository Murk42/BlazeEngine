#include "pch.h"
#include "BlazeEngineCore/Memory/MemoryManager.h"
#include "BlazeEngine/Application/ECS/Scene.h"

#include "EntityCreationData.h"
#include "SystemCreationData.h"

namespace Blaze::ECS
{
	Scene::Scene()	
		: registry(ComponentTypeRegistry::NewRegistry())
	{
	}
	Scene::~Scene()
	{
		Clear();
	}
	void Scene::SetRegistry(ComponentTypeRegistry registry)
	{
		this->registry = std::move(registry);

		auto allTypes = this->registry.GetAllTypesData();

		systems.Resize(allTypes.Count());
		containers.Resize(allTypes.Count());

		for (int i = 0; i < allTypes.Count(); ++i)
		{
			auto& typeData = allTypes[i];

			void* systemRaw = Memory::Allocate(typeData.SystemSize());
			systems[i] = (System*)((uint8*)systemRaw + typeData.SystemBaseOffset());			

			containers[i].SetTypeData(allTypes[i]);

			systemCreationData.scene = this;
			systemCreationData.typeData = &allTypes[i];			

			allTypes[i].ConstructSystem(systems[i]);			

			systemCreationData.scene = nullptr;
			systemCreationData.typeData = nullptr;			
		}				
	}
	void Scene::Clear()
	{
		for (auto& container : containers)
			container.Clear();

		for (const auto& entity : entities)
		{
			std::destroy_at(entity);
			Memory::Free(entity);
		}

		auto types = registry.GetAllTypesData();

		for (uintMem i = 0; i < types.Count(); ++i)
		{
			void* systemRaw = (uint8*)systems[i] - types[i].SystemBaseOffset();
			types[i].DestructSystemDirect(systemRaw);
			Memory::Free(systemRaw);
		}
		
		registry = ComponentTypeRegistry::NewRegistry();
		entities.Clear();
	}
	Entity* Scene::Create(ArrayView<const ComponentTypeData*> typesData)
	{		
		auto* entity = CreateEntity(typesData);
		AllocateComponents();

		for (uintMem i = 0; i < typesData.Count(); ++i)
		{			
			Component* component = GetCurrentComponent();

			typesData[i]->Construct(component);			
		}

		FinishEntityCreation();

		return entity;
	}
	
	void Scene::Destroy(Entity* entity)
	{
		if (entity == nullptr)
			return;

		if (entity->scene != this)
		{
			BLAZE_ENGINE_ERROR("BlazeEngine", "Trying to delete a entity from a scene that it doesn't belong to.");
			return;
		}

		auto componentsTypeData = GetEntityComponentsTypeData(entity);
		auto components = GetEntityComponents(entity);
		uintMem componentCount = entity->GetComponentCount();

		for (uintMem i = 0; i < componentCount; ++i)
			systems[componentsTypeData[i]->Index()]->Destroyed(components[i]);

		for (uintMem i = 0; i < componentCount; ++i)
			containers[componentsTypeData[i]->Index()].Destroy(components[i]);

		entities.Last()->arrayIndex = entity->arrayIndex;
		entities[entity->arrayIndex] = entities.Last();
		entities.EraseLast();		

		std::destroy_at(entity);

		Memory::Free(entity);		
	}
	void Scene::UpdateSystem(const ComponentTypeData& typeData)
	{
		UpdateSystem(typeData.Index());
	}

	void Scene::UpdateSystem(uintMem index)
	{
		auto* system = systems[index];

		if (system->PreUpdate())
		{
			auto& container = containers[index];

			for (auto component : container)
				system->Update(component);

			system->PostUpdate();
		}		
	}

	System* Scene::GetSystem(const ComponentTypeData& typeData)
	{
		return GetSystem(typeData.Index());
	}	
	System* Scene::GetSystem(uintMem index)
	{
		return systems[index];
	}
	const ComponentContainer& Scene::GetComponents(const ComponentTypeData& typeData)
	{
		return containers[typeData.Index()];
	}
	ArrayView<Entity*> Scene::GetEntities() const
	{
		return ArrayView<Entity*>(entities.Ptr(), entities.Count());
	}


	Entity* Scene::CreateEntity(ArrayView<const ComponentTypeData*> typesData)
	{		
		currentEntityCreationData = &*entityCreationData.AddFront();
		currentEntityCreationData->scene = this;
		currentEntityCreationData->typesData = typesData;
		currentEntityCreationData->systems = systems.Ptr();

		Entity* entity = (Entity*)Memory::Allocate(sizeof(Entity) + (sizeof(Component*) + sizeof(ComponentTypeData*)) * typesData.Count());		
		std::construct_at(entity);
		entity->arrayIndex = entities.Count();
		entities.AddBack(entity);			

		currentEntityCreationData->currentTypeData = GetEntityComponentsTypeData(entity);
		currentEntityCreationData->currentEntity = entity;		

		auto componentData = GetEntityComponentsTypeData(entity);

		for (uintMem i = 0; i < typesData.Count(); ++i)
		{			
			componentData[i] = typesData[i];
		}

		return entity;
	}
	ECS::Component** Scene::AllocateComponents()
	{		
		auto typesData = currentEntityCreationData->typesData;
		auto components = GetEntityComponents(currentEntityCreationData->currentEntity);

		for (uintMem i = 0; i < typesData.Count(); ++i)
			components[i] = containers[typesData[i]->Index()].Allocate();		

		return components;
	}
	Component* Scene::GetCurrentComponent()
	{
		auto entity = currentEntityCreationData->currentEntity;
		auto componentsTypeData = GetEntityComponentsTypeData(entity);
		auto components = GetEntityComponents(entity);
		return components[currentEntityCreationData->currentTypeData - componentsTypeData];
	}
	void Scene::FinishEntityCreation()
	{		
		auto entity = currentEntityCreationData->currentEntity;
		auto componentsTypeData = GetEntityComponentsTypeData(currentEntityCreationData->currentEntity);
		auto components = GetEntityComponents(currentEntityCreationData->currentEntity);

		uintMem componentCount = entity->componentCount;
		for (uintMem i = 0; i < componentCount; ++i)
			systems[componentsTypeData[i]->Index()]->Created(components[i]);
		
		entityCreationData.EraseFirst();		

		if (entityCreationData.Empty())
			currentEntityCreationData = nullptr;
		else
			currentEntityCreationData = &entityCreationData.First();
	}	
}