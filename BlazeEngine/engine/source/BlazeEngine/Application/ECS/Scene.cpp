#include "BlazeEngine/Application/ECS/Scene.h"

#include "ComponentConstructData.h"
#include "SystemConstructData.h"

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
	Result Scene::SetRegistry(ComponentTypeRegistry registry)
	{
		this->registry = std::move(registry);

		auto allTypes = this->registry.GetAllTypesData();

		systems.Resize(allTypes.size());
		containers.Resize(allTypes.size());

		for (int i = 0; i < allTypes.size(); ++i)
		{
			auto& typeData = allTypes[i];

			void* ptr = Memory::Allocate(typeData.SystemSize());
			systems[i] = (System*)((uint8*)ptr + typeData.SystemBaseOffset());

			containers[i].SetTypeData(allTypes[i]);
		}

		for (int i = 0; i < allTypes.size(); ++i)
		{
			systemConstructData.scene = this;
			systemConstructData.typeData = &allTypes[i];
			systemConstructData.state = 1;

			allTypes[i].ConstructSystem(systems[i]);

			if (systemConstructData.state != 2)
			{
				Logger::ProcessLog(BLAZE_WARNING_LOG("Blaze Engine",
					"Internal engine error. systemsConstructData.state was changed inappropriately. The systems component type was: \"" + allTypes[i].GetTypeName() + "\""));
			}

			systemConstructData.scene = nullptr;
			systemConstructData.typeData = nullptr;
			systemConstructData.state = 0;
		}
		

		return Result();
	}
	void Scene::Clear()
	{
		entities.Clear();
	}
	Entity* Scene::Create(std::initializer_list<const ComponentTypeData*> componentsTypeData)
	{		
		return Create(componentsTypeData, nullptr, nullptr);
	}
	Entity* Scene::Create(std::initializer_list<const ComponentTypeData*> componentsTypeData, Component** components)
	{	
		return Create(componentsTypeData, components, nullptr);		
	}
	Entity* Scene::Create(std::initializer_list<const ComponentTypeData*> componentsTypeData, Component** components, std::function<void(void*)>* constructors)
	{		
		Entity* ptr = entities.Allocate();

		ptr->scene = this;
		ptr->SetupComponents(componentsTypeData);		

		for (uint i = 0; i < componentsTypeData.size(); ++i)
		{
			auto typeData = componentsTypeData.begin()[i];

			Component* component = containers[typeData->Index()].Allocate();
			ptr->SetComponent(i, component);

			if (components != nullptr)
				components[i] = component;
		}
		
		for (uint i = 0; i < componentsTypeData.size(); ++i)
		{			
			auto typeData = componentsTypeData.begin()[i];

			componentConstructData.entity = ptr;			
			componentConstructData.state = 1;			

			if (constructors != nullptr && constructors[i])
				constructors[i](ptr->components[i] - typeData->BaseOffset());	///USER CODE CALLED
			else
				typeData->Construct(ptr->components[i]);		///USER CODE CALLED

			if (componentConstructData.state == 1)
			{
				Logger::ProcessLog(BLAZE_WARNING_LOG("Blaze Engine",
					"Component constructor not called while creating an entity with custom component constructors. Or there was an internal engine error. The component type was: \"" + typeData->GetTypeName() + "\""));
			}
			else if (componentConstructData.state != 2)
			{
				Logger::ProcessLog(BLAZE_WARNING_LOG("Blaze Engine",
					"Internal engine error. componentsConstructData.state was changed inappropriately after calling a custom component constructor. The component type was: \"" + typeData->GetTypeName() + "\""));
			}

			componentConstructData.entity = nullptr;			
			componentConstructData.state = 0;
		}

		for (uint i = 0; i < componentsTypeData.size(); ++i)
		{
			auto typeData = componentsTypeData.begin()[i];
			systems[typeData->Index()]->Created(ptr->components[i]);			
		}

		return ptr;
	}
	Result Scene::Destroy(Entity* entity)
	{
		if (entity == nullptr)
			return Result();		

		if (entity->scene != this)
			return BLAZE_ERROR_RESULT("BlazeEngine", "Trying to delete a entity from a scene that it doesn't belong to.");
		
		auto components = entity->GetComponents();

		for (uint i = 0; i < components.size(); ++i)
		{
			auto& typeData = components[i]->GetTypeData();
			systems[typeData.Index()]->Destroyed(components[i]);
		}

		for (uint i = 0; i < components.size(); ++i)
		{
			auto& typeData = components[i]->GetTypeData();
			containers[typeData.Index()].Destroy(components[i]);
		}

		entities.Clear();
		return Result();
	}
	Result Scene::UpdateSystem(const ComponentTypeData& typeData)
	{
		systems[typeData.Index()]->Update(containers[typeData.Index()]);
		return Result();
	}

	System* Scene::GetSystem(const ComponentTypeData& typeData)
	{
		return systems[typeData.Index()];
	}	
	const ComponentContainer& Scene::GetComponents(const ComponentTypeData& typeData)
	{
		return containers[typeData.Index()];
	}
	//void Scene::Render()
	//{
	//	for (uint i = 0; i < manager->GetRegistry().GetComponentTypeCount(); ++i)
	//		manager->GetSystem(i)->Render(containers[i]);
	//}
}