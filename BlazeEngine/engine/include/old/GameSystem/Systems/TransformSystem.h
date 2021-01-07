#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/GameSystem/ECS/System.h"
#include "BlazeEngine/GameSystem/Components/Transform.h"

namespace Blaze
{
	class BLAZE_API TransformSystem : public System<TransformSystem, Transform>
	{
	public:
		void Update(Array<Transform> transforms);
	};
}