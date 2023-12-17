#include "BlazeEngine/Graphics/Transform.h"

namespace Blaze::Graphics
{
    Mat4f Transform2D::GetMatrix(Vec2f size) const
    {			
		Vec2f finalPos = pos;
		Vec2f finalSize = size;
		finalPos -= pivot * finalSize;

		finalSize = Vec2f(std::floor(finalSize.x), std::floor(finalSize.y));
		finalPos = Vec2f(std::floor(finalPos.x), std::floor(finalPos.y));

		Mat4f matrix = Mat4f::TranslationMatrix(Vec3f(finalPos.x, finalPos.y, 0)) * Mat4f::RotationMatrixAxisZ(rotation);

		return matrix;
    }
}