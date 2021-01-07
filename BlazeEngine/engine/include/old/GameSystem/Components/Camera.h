#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/GameSystem/ECS/Component.h"
#include "BlazeEngine/DataStructures/Matrix.h"

namespace Blaze
{	
	class BLAZE_API Camera : public Component<Camera>
	{
	public:		
		Mat4f projectionMatrix;

		Camera() = default;
		Camera(float fov, float aspectRatio, float near, float far);
		Camera(float left, float right, float bottom, float top, float near, float far);
	};	
}