#pragma once

namespace Blaze::ECS
{
	class Scene;
	class ComponentTypeData;

	struct SystemCreationData
	{
		Scene* scene;
		const ComponentTypeData* typeData;		
	};

	extern SystemCreationData systemCreationData;
}