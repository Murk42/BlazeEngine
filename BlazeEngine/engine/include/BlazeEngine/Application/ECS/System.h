#pragma once
#include "BlazeEngine/Application/ECS/ComponentTypeRegistry.h"

namespace Blaze::ECS
{
	class Component;
	class ComponentContainer;
	class Scene;

	class BLAZE_API System
	{
		const ComponentTypeData& typeData;
		Scene* scene;
	public:
		System();		

		virtual void Created(Component*);
		virtual void Destroyed(Component*);

		virtual void Update(const ComponentContainer&);
		virtual void Update(Component* component);		

		inline Scene* GetScene() const { return scene; }
		inline const ComponentTypeData& GetTypeData() const { return typeData; }

		template<typename T>
		T::System* Cast()
		{
			if (typeData->GetTypeName().Ptr() != T::typeName)
			{
				Logger::ProcessLog(BLAZE_FATAL_LOG("Blaze Engine", "Invalid component cast"));
				return nullptr;
			}

			return (typename T::System*)this;
		}

		friend class Scene;
	};
}

/*
Ordered rendering

A, B, B', C, C, B', A, A', C, B

o = 0
iteration:
 x = value of first next ' with value >o
 if none
  resolve from o to x
 else 
  iteration

0 A1, A7, A8'
0 B2, B3', B6', B10
0 C4, C5, C9

8

2 A1, A7, A8'
0 B2, B3', B6', B10
0 C4, C5, C9

8
3

2 A1, A7, A8'
0 B2, B3', B6', B10
0 C4, C5, C9

8

2 A7, A8'
1 B6', B10
3 C5, C9

8
6

2 A7, A8'
1 B6', B10
3 C5, C9

*/