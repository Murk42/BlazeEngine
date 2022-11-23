#pragma once
#include "Component.h"

namespace Blaze::ECS
{
	class Entity
	{
		Array<Component*> components;
		uint bucketIndex;
	public:
		friend class Scene;
	};
}