#pragma once
#define BLAZE_ONLY_CORE
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

#include "PhysicsObject.h"
#include "Camera.h"
#include "Utilities.h"
#include "Controller.h"

struct SpaceShipSpecs
{
};

class SpaceShipObject : public PhysicsObject
{	
public:		
	float maxThrust;	

	SpaceShipObject()
	{

	}
	SpaceShipObject(float maxThrust)
		: maxThrust(maxThrust)
	{

	}

	void Update() override
	{			
		PhysicsObject::Update();
	}
};

class SpaceShipController : public Controller
{
	bool addedThrust;
	Console::VariableFrame* vf;
public:
	Camera* camera;
	SpaceShipObject* ship;	
	vec3f shipRotations;	
	float thrust;	

	SpaceShipController(Camera* camera, SpaceShipObject* ship, bool active = true)
		: Controller(&ship->tran, active), camera(camera), ship(ship)
	{

	}

	void SetupVariableFrame(Console::VariableFrame* vf)
	{
		this->vf = vf;

		vf->Add("Thrust", &thrust);
		vf->Add("Velocity", &ship->velocity.z);
	}

	void AddThrust(float amount)
	{				
		MaxOne(MaxInf(thrust) + amount * 0.2f);
		float temp = 50 * thrust / (1.0f - thrust) + amount;
		thrust = 1.0f - 1.0f / (temp * 0.02f + 1.0f);
		addedThrust = true;
	}	

	void Update() override
	{
		if (!active)return;

		if (Input::GetKey(Key::W).value == KeyDown)
		{
			float amount = Profiler::dt * 10;
			MaxOne(MaxInf(thrust) + amount * 0.2f);
			float temp = 50 * thrust / (1.0f - thrust) + amount;
			thrust = 1.0f - 1.0f / (temp * 0.02f + 1.0f);
			addedThrust = true;
		}

		{
			vec2f& rot = As<vec2f>(shipRotations);
			if (Input::GetKey(Key::MouseMiddle).value == KeyDown)
			{
				vec2f m = Input::GetMouseMovement();
				rot += m * 0.004;
				float lensqr = rot.x * rot.x + rot.y * rot.y;
				if (lensqr > 1)
					rot = rot / Math::Sqrt(lensqr);
			}
			else
			{
				rot *= (1.0f - Profiler::dt * 5);
			}

			if (Input::GetKey(Key::D).value == KeyDown)
			{
				shipRotations.z -= Profiler::dt * 4;
				if (shipRotations.z < -1)
					shipRotations.z = -1;
			}
			else if (Input::GetKey(Key::A).value == KeyDown)
			{
				shipRotations.z += Profiler::dt * 4;
				if (shipRotations.z > 1)
					shipRotations.z = 1;
			}
			else
			{
				shipRotations.z *= (1.0f - Profiler::dt * 5);
			}

			ship->tran.rot *= Math::ToQuaternion(vec4f(1, 0, 0, shipRotations.y * 0.002f));
			ship->tran.rot *= Math::ToQuaternion(vec4f(0, 1, 0, shipRotations.x * 0.002f));
			ship->tran.rot *= Math::ToQuaternion(vec4f(0, 0, 1, shipRotations.z * 0.002f));
		}

		ship->velocity += ship->tran.rot * vec3f(0, 0, 1) * thrust * ship->maxThrust * Profiler::dt;
		vf->Clear();
		vf->Refresh();

		if (!addedThrust)
		{
			thrust -= Profiler::dt * 0.5f;
			if (thrust < 0)
				thrust = 0;
		}
		else
			addedThrust = false;

		camera->tran.pos = ship->tran.pos + ship->tran.rot * vec3f(0, 3, -6);
		camera->tran.rot = ship->tran.rot;
		camera->Projection.Perspective.fov = 90 + (1.0f - 1.0f / (thrust + 1)) * 40;
	}
};