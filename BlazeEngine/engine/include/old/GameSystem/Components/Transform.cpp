#include "Transform.h"

namespace Blaze
{
	Transform::Transform()
		: mat(Mat4f::Identity())
	{
	}
	Transform::Transform(const Transform& t)
		: mat(t.mat)
	{
	}
}
