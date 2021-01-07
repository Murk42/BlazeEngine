#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/GameSystem/ECS/Component.h"
#include "BlazeEngine/GameSystem/Components/Transform.h"

#include "BlazeEngine/DataStructures/Vector.h"
#include "BlazeEngine/DataStructures/Quaternion.h"


namespace Blaze
{
	class BLAZE_API Transform3D : public Component<Transform3D, Transform>, public Transform
	{
	public:		
		Vec3f pos;
		Vec3f scale;
		Quatf rot;

		Transform3D();
		Transform3D(const Transform3D&);
		Transform3D(const Vec3f& pos, const Vec3f& scale, const Quatf& rot);

		void UpdateMatrix() override;
	};

}