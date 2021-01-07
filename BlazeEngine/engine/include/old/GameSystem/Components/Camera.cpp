#include "Camera.h"
#include "BlazeEngine/Math/Math.h"

namespace Blaze
{			
	Camera::Camera(float fov, float aspectRatio, float near, float far)
		: projectionMatrix(Math::PerspectiveMatrix<float>(fov, aspectRatio, near, far))
	{

	}
	Camera::Camera(float left, float right, float bottom, float top, float near, float far)
		: projectionMatrix(Math::OrthographicMatrix<float>(left, right, bottom, top, near, far))
	{

	}
}