#pragma once
#include "BlazeEngine/DataStructures/Matrix.h"
#include "BlazeEngine/Math/Math.h"

namespace Blaze
{	 
	class BLAZE_API Transform3D
	{
	public:
		Vec3f pos;
		Quatf rot;
		Vec3f scale;		

		Vec3f absolutePos;
		Quatf absoluteRot;
		Vec3f absoluteScale;
		
		Transform3D* parent;

		Transform3D();
		Transform3D(Vec3f pos, Quatf rot, Vec3f scale, Transform3D* parent = nullptr);
		
		void RotateAround(Vec3f center, Quatf rot);		

		static Mat4f CalculateTransformMatrix(Transform3D&);
		static Mat4f CalculateViewMatrix(Transform3D&);
	};
}