#include "BlazeEngine/DataStructures/Transform3D.h"
#include "BlazeEngine/Graphics/Renderer.h"

namespace Blaze
{
	Transform3D::Transform3D()
		: pos(0), rot(), scale(1), parent(nullptr)
	{

	}
	Transform3D::Transform3D(Vec3f pos, Quatf rot, Vec3f scale, Transform3D* parent)
		: pos(pos), rot(rot), scale(scale), parent(parent)
	{
	}

	void Transform3D::RotateAround(Vec3f center, Quatf rot)
	{
		this->pos = rot * (this->pos - center) + center;
		this->rot *= rot;
	}

	void Transform3D::Update()
	{
		if (parent != nullptr)
		{			
			absolutePos = parent->absolutePos + pos;
			absoluteRot = parent->absoluteRot * rot;
			absoluteScale = parent->absoluteScale * scale;
			mat = Math::TranslationMatrix<float>(absolutePos) * Math::RotationMatrix(absoluteRot) * Math::ScalingMatrix(absoluteScale);
		}
		else
		{			
			absolutePos = pos;
			absoluteRot = rot;
			absoluteScale = scale;
			mat = Math::TranslationMatrix<float>(absolutePos) * Math::RotationMatrix(absoluteRot) * Math::ScalingMatrix(absoluteScale);
		}
	}
	void Transform3D::UpdateAsViewTransform()
	{
		if (parent != nullptr)
		{
			absolutePos = parent->absolutePos + pos;
			absoluteRot = parent->absoluteRot * rot;
			absoluteScale = parent->absoluteScale * scale;
			mat = Math::RotationMatrix(absoluteRot.Conjugated()) * Math::TranslationMatrix<float>(-absolutePos) * Math::ScalingMatrix(absoluteScale);
		}
		else
		{
			absolutePos = pos;
			absoluteRot = rot;
			absoluteScale = scale;
			mat = Math::RotationMatrix(absoluteRot.Conjugated()) * Math::TranslationMatrix<float>(-absolutePos) * Math::ScalingMatrix(absoluteScale);
		}
	}
}