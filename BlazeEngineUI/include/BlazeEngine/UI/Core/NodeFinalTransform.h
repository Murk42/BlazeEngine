#pragma once
#include "BlazeEngine/Core/Math/Vector.h"

namespace Blaze::UI
{
	struct BLAZE_API NodeFinalTransform
	{
		Vec2f position;
		Vec2f size;
		float rotation = 0.0f;
		Vec2f right;

		inline Vec2f Right() const
		{
			return right;
		}
		inline Vec2f Up() const
		{
			return Vec2f(-right.y, right.x);
		}

		bool operator==(const NodeFinalTransform&) const = default;
		bool operator!=(const NodeFinalTransform&) const = default;
	};
}