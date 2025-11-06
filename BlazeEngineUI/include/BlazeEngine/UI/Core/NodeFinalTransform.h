#pragma once
#include "BlazeEngine/Core/Math/Vector.h"

namespace Blaze::UI
{
	struct BLAZE_API NodeFinalTransform
	{
		Vec2f position;
		Vec2f size;
		float scale = 1.0f;
		float rotation = 0.0f;

		Vec2f TransformFromFinalToLocalTransformSpace(Vec2f finalTransformPosition) const;
		void GetRectVectors(Vec2f& position, Vec2f& right, Vec2f& up) const;

		bool operator==(const NodeFinalTransform&) const = default;
		bool operator!=(const NodeFinalTransform&) const = default;
	};
}