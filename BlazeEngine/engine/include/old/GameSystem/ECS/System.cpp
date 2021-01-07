#include "System.h"

#include "BlazeEngine/GameSystem/Systems/RenderSystem.h"
#include "BlazeEngine/GameSystem/Components/Camera.h"
#include "BlazeEngine/GameSystem/Components/Model.h"
#include "BlazeEngine/GameSystem/Components/Transform3D.h"
#include "BlazeEngine/GameSystem/ECS/ECS.h"

#include "BlazeEngine/Internal/TypeStructs.h"

namespace Blaze
{			
	extern std::vector<SystemType> systemTypes;
	extern std::vector<ComponentType> componentTypes;

	BaseSystem::BaseSystem(SystemTypeID typeID)
		: typeID(typeID)
	{

	}

	inline void BaseSystem::InitializeTypes(std::vector<TypeData>& types, size_t typeHash, bool isComponent)
	{
		if (isComponent)
			for (ComponentTypeID i = 0; i < componentTypes.size(); ++i)
			{
				auto& t = componentTypes[i];
				if (t.hash == typeHash)
				{
					types.emplace_back(TypeData({ i, t.size, t.baseOffset, nullptr, nullptr }));
					return;
				}
			}
		else
			for (ComponentTypeID i = 0; i < componentTypes.size(); ++i)
			{
				auto& t = componentTypes[i];
				auto it = std::find_if(t.bases.begin(), t.bases.end(), [&](std::pair<size_t, size_t>& p) { return p.first == typeHash; });
				if (it != t.bases.end())
					types.emplace_back(TypeData({ i, t.size, it->second, nullptr, nullptr }));
			}
	}

	inline void BaseSystem::UpdateTypes(std::vector<TypeData>& types)
	{
		for (auto& t : types)
		{
			t.begin = ecs->components[t.typeID].begin()._Ptr;
			t.end = ecs->components[t.typeID].end()._Ptr;
		}
	}

	SystemTypeID BaseSystem::RegisterSystemType(Func<> construct, Func<> destruct, size_t size, size_t offset, Func<> update)
	{
		systemTypes.push_back({ construct, destruct, size, offset, update});
		return systemTypes.size() - 1;		
	}	
}