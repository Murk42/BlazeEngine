#pragma once

namespace Blaze::ECS
{
	class Scene;
	class ComponentTypeData;

	struct SystemConstructData
	{
		Scene* scene;
		const ComponentTypeData* typeData;
		uint state;
	};

	extern SystemConstructData systemConstructData;
}