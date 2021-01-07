#pragma once
#define BLAZE_ONLY_CORE
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

#include "Object.h"
#include "Transform.h"

class PhysicsObject : public Object
{
public:
	vec3f velocity;
	float mass;

	void ApplyForce(vec3f direction, float intensity)
	{
		velocity += direction * (intensity / mass);
	}

	float GetSpeed() const
	{
		return Math::Lenght(velocity);
	}

	void Update() override
	{
		tran.pos += velocity * Profiler::dt;

		Object::Update();
	}
};