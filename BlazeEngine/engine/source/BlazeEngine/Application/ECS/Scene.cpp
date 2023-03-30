#include "BlazeEngine/Application/ECS/Scene.h"

namespace Blaze::ECS
{
	Scene::Scene()
		: manager(nullptr)
	{
	}
	Scene::~Scene()
	{
		Clear();
	}
	Result Scene::SetManager(const Manager* manager)
	{
		if (this->manager != nullptr)
			return BLAZE_ERROR_RESULT("Blaze Engine", "Cant change manager");

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
		entities.Clear();
	}
	Entity* Scene::Create(std::initializer_list<uint> componentTypeIndexes)
	{		
		return Create(componentTypeIndexes, nullptr);
	}
	Entity* Scene::Create(std::initializer_list<uint> componentTypeIndexes, Component** components)
	{
		auto& registry = manager->GetRegistry();

		if (componentTypeIndexes.size() > registry.GetComponentTypeCount())
		{
			Logger::AddLog(BLAZE_ERROR_LOG("Blaze Engine", "invalid component type indicies"));
			return nullptr;
		}

		Entity* ptr = entities.Allocate();

		ptr->scene = this;		

		ptr->components.Resize(componentTypeIndexes, registry.GetComponentTypeCount());

		uint i = 0;
		for (auto& typeIndex : componentTypeIndexes)
		{
			Component* component = containers[typeIndex].Create();
			component->typeIndex = typeIndex;
			component->entity = ptr;

			ptr->components.SetComponent(component);

			if (components != nullptr)
				components[i] = component;
			
			++i;
		}

		for (auto component : ptr->components)
			manager->GetSystem(component->typeIndex)->Created(component);		

		return ptr;
	}
	Result Scene::Destroy(Entity* entity)
	{
		if (entity == nullptr)
			return Result();				

		for (auto component : entity->components)
			manager->GetSystem(component->typeIndex)->Destroyed(component);

		for (auto component : entity->components)
			containers[component->typeIndex].Destroy(component);

		entities.Clear();
		return Result();
	}
	void Scene::UpdateSystem(uint typeIndex)
	{				
		manager->GetSystem(typeIndex)->Update(containers[typeIndex]);
	}
	void Scene::Render()
	{
		for (uint i = 0; i < manager->GetRegistry().GetComponentTypeCount(); ++i)
			manager->GetSystem(i)->Render(containers[i]);
	}
}