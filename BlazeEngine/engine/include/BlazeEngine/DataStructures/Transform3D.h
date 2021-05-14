#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Vector.h"
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
		Mat4f mat;

		Vec3f absolutePos;
		Quatf absoluteRot;
		Vec3f absoluteScale;
		
		Transform3D* parent;

		Transform3D();
		Transform3D(Vec3f pos, Quatf rot, Vec4f scale, Transform3D* parent = nullptr);

		void Update();
		void UpdateAsViewTransform();
	};
}