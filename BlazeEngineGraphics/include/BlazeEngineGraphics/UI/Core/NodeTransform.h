#pragma once
#include "BlazeEngineCore/Math/Vector.h"
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"

namespace Blaze::UI
{
	struct BLAZE_GRAPHICS_API NodeTransform
	{
		Vec2f pos = Vec2f(0.0f, 0.0f);
		Vec2f parentPivot = Vec2f(0.5f, 0.5f);
		Vec2f pivot = Vec2f(0.5f, 0.5f);
		Vec2f size = Vec2f(0.0f, 0.0f);
		float scale = 1.0f;
		float rotation = 0.0f;

		bool operator==(const NodeTransform& other) const = default;
		bool operator!=(const NodeTransform& other) const = default;
	};
}