#include "BlazeEngine/DataStructures/Transform3D.h"
#include "BlazeEngine/Graphics/GraphicsCore.h"

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

	Mat4f Transform3D::CalculateTransformMatrix(Transform3D& t)
	{
		if (t.parent != nullptr)
		{			
			t.absolutePos = t.parent->absolutePos + t.pos;
			t.absoluteRot = t.parent->absoluteRot * t.rot;
			t.absoluteScale = t.parent->absoluteScale * t.scale;
			return Math::TranslationMatrix<float>(t.absolutePos) * Math::RotationMatrix(t.absoluteRot) * Math::ScalingMatrix(t.absoluteScale);
		}
		else
		{			
			t.absolutePos = t.pos;
			t.absoluteRot = t.rot;
			t.absoluteScale = t.scale;
			return Math::TranslationMatrix<float>(t.absolutePos) * Math::RotationMatrix(t.absoluteRot) * Math::ScalingMatrix(t.absoluteScale);
		}
	}	
	Mat4f Transform3D::CalculateViewMatrix(Transform3D& t)	
	{
		if (t.parent != nullptr)
		{
			t.absolutePos = t.parent->absolutePos + t.pos;
			t.absoluteRot = t.parent->absoluteRot * t.rot;
			t.absoluteScale = t.parent->absoluteScale * t.scale;
			return Math::RotationMatrix(t.absoluteRot.Conjugated()) * Math::TranslationMatrix<float>(-t.absolutePos) * Math::ScalingMatrix(t.absoluteScale);
		}
		else
		{
			t.absolutePos = t.pos;
			t.absoluteRot = t.rot;
			t.absoluteScale = t.scale;
			return Math::RotationMatrix(t.absoluteRot.Conjugated()) * Math::TranslationMatrix<float>(-t.absolutePos) * Math::ScalingMatrix(t.absoluteScale);
		}
	}
}