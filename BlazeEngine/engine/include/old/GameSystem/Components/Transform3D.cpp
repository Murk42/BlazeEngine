#pragma once
#include "Transform3D.h"
#include "BlazeEngine/Math/Math.h"

namespace Blaze
{
	Transform3D::Transform3D()
		: Transform(), pos(0, 0, 0), scale(1, 1, 1), rot(0, 0, 0, 1)
	{

	}
	Transform3D::Transform3D(const Transform3D& t)
		: Transform(t), pos(t.pos), scale(t.scale), rot(t.rot)
	{
	}
	Transform3D::Transform3D(const Vec3f& pos, const Vec3f& scale, const Quatf& rot)
		: pos(pos), scale(scale), rot(rot)
	{
	}
	void Transform3D::UpdateMatrix()
	{
		mat = Math::TransformMatrix(pos, scale, rot);
	}

}