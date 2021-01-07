#pragma once
#define BLAZE_ONLY_CORE
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

struct Transform
{
	mat4f mat;
	vec3f pos;
	vec3f scale;
	quatf rot;
	
	Transform()
		: scale(1)
	{

	}
	Transform(const vec3f& pos, const vec3f& scale, const quatf& rot)
		: pos(pos), scale(scale), rot(rot)
	{

	}

	void UpdateMatrix()
	{
		mat = Math::TranslationMatrix(pos) * Math::ScalingMatrix(scale) * Math::RotationMatrix(rot);
	}
};