#pragma once
#include "BlazeEngineCore/Math/Matrix.h"

namespace Blaze::Graphics
{
	struct BLAZE_CORE_API Transform2D
	{
		Vec2f pos = Vec2f(0, 0);
		float rotation = 0;
		Vec2f pivot = Vec2f(0, 1);

		Mat4f GetMatrix(Vec2f size) const;
	};
}