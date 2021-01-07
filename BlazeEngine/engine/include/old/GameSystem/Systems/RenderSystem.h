#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/GameSystem/ECS/System.h"

#include "BlazeEngine/GameSystem/Components/Camera.h"
#include "BlazeEngine/GameSystem/Components/Model.h"
#include "BlazeEngine/GameSystem/Components/Transform3D.h"

namespace Blaze
{	
	class BLAZE_API RenderSystem : public System<RenderSystem, Camera, Model, Transform3D>
	{
	public:
		void Update(Array<Camera>, Array<Model>, Array<Transform3D>);
	};	
}