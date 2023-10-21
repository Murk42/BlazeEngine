#pragma once
#include "Component.h"
#include "ComponentTypeRegistry.h"

namespace Blaze::ECS
{
	class Scene;

	class BLAZE_API Entity
	{
	public:
		Entity();
		~Entity();

		template<typename C> requires IsComponent<C>
		bool HasComponent() const;
		bool HasComponent(const ComponentTypeData&) const;		

		template<typename C> requires IsComponent<C>
		C* GetComponent() const;
		Component* GetComponent(const ComponentTypeData&) const;		
		Component* GetComponent(uintMem index) const;
		const ComponentTypeData* GetComponentTypeData(uintMem index);
				
		uintMem GetComponentCount() const { return componentCount; }

		Scene* GetScene() const { return scene; }

		friend class Scene;		
	private:				
		Scene* scene;				
		uintMem componentCount;
		uintMem arrayIndex;

		const ComponentTypeRegistry* GetRegistry() const;
	};

	template<typename C> requires IsComponent<C>
	bool Entity::HasComponent() const
	{
		auto registry = GetRegistry();		
		return HasComponent(registry->GetComponentTypeData<C>());
	}

	template<typename C> requires IsComponent<C>
	C* Entity::GetComponent() const
	{
		auto registry = GetRegistry();		
		const ComponentTypeData* typeData;

		if (registry->GetComponentTypeData<C>(typeData))
			return (C*)GetComponent(*typeData);		
		{
			Debug::Logger::LogError("Blaze Engine", "Registry doesnt contain this component type");
			return nullptr;
		}
	}
}

/*
	0 1 2 3 4 5 6 7 8 9 10     900
	A B C D E F G H I J K  ...  Z
	.   . .         .   .       . 6
	. .   .   . . .               6
	    . .                     . 3
	.   .   .     .   .           5	
*/