#include "pch.h"
#include "BlazeEngineGraphics/UI/Core/NodeFinalTransform.h"

namespace Blaze::UI
{
	Vec2f NodeFinalTransform::TransformFromFinalToLocalTransformSpace(Vec2f finalTransformPosition)
	{
		Vec2f localTransformPosition = (finalTransformPosition - position) / scale;

		if (rotation != 0)
		{
			float cos = Math::Cos(rotation);
			float sin = Math::Sin(rotation);
			Vec2f right = Vec2f(cos, sin);
			Vec2f up = Vec2f(-sin, cos);

			localTransformPosition = Vec2f(right.DotProduct(localTransformPosition), up.DotProduct(localTransformPosition));
		}

		return localTransformPosition;
	}
}