#pragma once
#define BLAZE_ONLY_CORE
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

#include "Camera.h"

enum class ShakeType
{
	Bump, Decreasing, Increasing
};

class CameraController
{
	inline static Timer dtTimer;
	inline static double dt;

	inline static bool shake = false;
	inline static Timer shakeTimer;
	inline static float shakeTime;
	inline static float shakeIntensity;		
	inline static ShakeType shakeType;
public:		
	inline static Camera* cam;
	inline static vec3f camRot;

	static void Shake(float time, float intensity, ShakeType type)
	{		
		shakeTimer.Reset();
		shakeTime = time;
		shakeIntensity = intensity;
		shakeType = type;
		shake = true;
	}

	static void Update()
	{								
		dt = dtTimer.Reset();		

		if (Input::GetKey(Key::J).value == KeyPressed) Shake(1, 1, ShakeType::Bump);
		if (Input::GetKey(Key::K).value == KeyPressed) Shake(1, 1, ShakeType::Decreasing);
		if (Input::GetKey(Key::L).value == KeyPressed) Shake(1, 1, ShakeType::Increasing);

		if (shake)
		{			
			static vec3f lastShake, shake, shakeSum;

			float time = shakeTimer.GetTime();
			if (time < shakeTime)
			{
				float t;
				switch (shakeType)
				{
				case ShakeType::Bump:       t = Math::Sin(time / shakeTime * (float)Math::PI); break;
				case ShakeType::Decreasing: t = Math::Sin((time / shakeTime + 1) * (float)Math::PI / 2); break;
				case ShakeType::Increasing: t = Math::Sin(-time / shakeTime * (float)Math::PI / 2); break;
				}

				shake.x = (Random::Float() * 2 - 1) * t * dt * 4 * shakeIntensity;
				shake.y = (Random::Float() * 2 - 1) * t * dt * 4 * shakeIntensity;
				shake.z = (Random::Float() * 2 - 1) * t * dt * 2 * shakeIntensity;

				cam->tran.pos += cam->tran.rot * vec3f(shake.x - lastShake.x, shake.y - lastShake.y, 0);				
				camRot.z += shake.z - lastShake.z;
				shakeSum += shake - lastShake;
				lastShake = shake;

				Console::MoveCursor(vec2i(0, 30));
				static char line[100];
				memset(line, ' ', 100);
				if (t > 0 && t < 1)
					line[(int)(t * 100)] = '*';

				Console::Write("t: |" + String(line, (size_t)100) + "|");				
			}
			else
			{
				cam->tran.pos -= vec3f(shakeSum.x, shakeSum.y, 0);
				camRot.z -= shakeSum.z;
				lastShake = 0;			
				shakeSum = 0;
				shake = false;
			}
		}

		//if (Input::GetKey(Key::W     ).value == KeyDown) cam->tran.pos += cam->tran.rot * vec3f( 0, 0, 1) * dt;
		//if (Input::GetKey(Key::S     ).value == KeyDown) cam->tran.pos += cam->tran.rot * vec3f( 0, 0,-1) * dt;
		//if (Input::GetKey(Key::Space ).value == KeyDown) cam->tran.pos += cam->tran.rot * vec3f( 0, 1, 0) * dt;
		//if (Input::GetKey(Key::LShift).value == KeyDown) cam->tran.pos += cam->tran.rot * vec3f( 0,-1, 0) * dt;
		//if (Input::GetKey(Key::D     ).value == KeyDown) cam->tran.pos += cam->tran.rot * vec3f( 1, 0, 0) * dt;
		//if (Input::GetKey(Key::A     ).value == KeyDown) cam->tran.pos += cam->tran.rot * vec3f(-1, 0, 0) * dt;
		//
		//if (Input::GetKey(Key::MouseMiddle).value == KeyDown)
		//{
		//	vec2i dp = Input::GetMouseMovement();
		//	camRot.x += dp.y * dt * 3;
		//	camRot.y += dp.x * dt * 3;
		//}		
		//cam->tran.rot = Math::ToQuaternion(vec4f(0, 1, 0, camRot.y)) * Math::ToQuaternion(vec4f(1, 0, 0, camRot.x)) * Math::ToQuaternion(vec4f(0, 0, 1, camRot.z));
	}
};