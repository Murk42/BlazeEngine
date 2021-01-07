#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Matrix.h"

namespace Blaze
{
	class Transform
	{
	public:
		Mat4f mat;

		Transform();
		Transform(const Transform&);

		virtual void UpdateMatrix() { }
	};
}