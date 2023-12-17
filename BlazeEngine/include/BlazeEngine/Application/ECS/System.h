#pragma once
#include "BlazeEngine/Application/ECS/ComponentTypeRegistry.h"

namespace Blaze::ECS
{
	class Component;
	class ComponentContainer;
	class Scene;

	class BLAZE_API System
	{
		const ComponentTypeData* typeData;
		Scene* scene;
	public:
		System();		

		virtual void Created(Component*);
		virtual void Destroyed(Component*);
		
		virtual bool PreUpdate() { return true; }
		virtual void Update(Component* component) { }
		virtual void PostUpdate() { }

		inline Scene* GetScene() const { return scene; }
		bool GetTypeData(const ComponentTypeData*& typeData) const;

		template<typename T>
		T::System* Cast()
		{
			if (typeData->GetTypeName().Ptr() != T::typeName)
			{
				Debug::Logger::LogError("Blaze Engine", "Invalid component cast");
				return nullptr;
			}

			return (typename T::System*)this;
		}

		friend class Scene;
	};	
}