#include "pch.h"
#include "BlazeEngine/UI/Core/NodeFinalTransform.h"

namespace Blaze::UI
{
	Vec2f NodeFinalTransform::TransformFromFinalToLocalTransformSpace(Vec2f finalTransformPosition) const
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
	void NodeFinalTransform::GetRectVectors(Vec2f& position, Vec2f& right, Vec2f& up) const
	{
		const float cos = Math::Cos(rotation);
		const float sin = Math::Sin(rotation);
		position = this->position;
		right = Vec2f(cos, sin) * size.x;
		up = Vec2f(-sin, cos) * size.y;
	}
}