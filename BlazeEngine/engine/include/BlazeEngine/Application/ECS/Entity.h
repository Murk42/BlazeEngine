#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Common.h"
#include "Component.h"

namespace Blaze::ECS
{
	class Scene;

	class BLAZE_API Entity
	{
		Scene* scene;
		ComponentGroup components;
		uint bucketIndex;

		const ComponentTypeRegistry& GetRegistry() const;
	public:
		const ComponentGroup& GetComponents() const { return components; };

		template<typename T>
		bool HasComponent() const 
		{
			return HasComponent(GetRegistry().GetComponentTypeIndex<T>());
		}
		bool HasComponent(uint typeIndex) const;

		template<typename T>
		T* GetComponent() const
		{
			return (T*)GetComponent(GetRegistry().GetComponentTypeIndex<T>());
		}
		Component* GetComponent(uint typeIndex) const;

		Scene* GetScene() const { return scene; }

		friend class Scene;
	};
}