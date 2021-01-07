#include "Scene.h"
#include "BlazeEngine/GameSystem/SceneManager.h"
#include "BlazeEngine/Core/Logger.h"
#include "BlazeEngine/Internal/TypeStructs.h"

namespace Blaze
{
	extern std::vector<SceneType> sceneTypes;
	extern std::vector<ResourceType> resourceTypes;	

	BaseScene::BaseScene(SceneTypeID typeID)
		: typeID(typeID)
	{		
	}

	BaseScene::~BaseScene()
	{		
	}

	SceneTypeID BaseScene::RegisterSceneType(Func<> construct, Func<> destruct, size_t size, size_t offset, Func<> load, Func<> unload, Func<> save, Func<> start, Func<float> update, Func<> stop)
	{		
		sceneTypes.push_back({ construct, destruct, size, offset, load, unload, save, start, update, stop });
		return sceneTypes.size() - 1;
	}

	inline Entity BaseScene::CreateEntity(String&& name, const std::initializer_list<ComponentTypeID>& list)
	{
		return ecs.CreateEntity(std::move(name), list);
	}
	inline Entity BaseScene::CreateEntity(String&& name, const std::initializer_list<ComponentTypeID>& list, const std::initializer_list<const BaseComponent*>& components)
	{
		return ecs.CreateEntity(std::move(name), list, components);
	}	

	inline void BaseScene::DeleteEntity(const Entity& entity)
	{
		ecs.DeleteEntity(entity);
	}

}
