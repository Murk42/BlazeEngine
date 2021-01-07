#pragma once
#define BLAZE_ONLY_CORE
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

class Camera;
class Transform;

class Renderable
{
public:
	virtual void Render(Camera*, Transform*) = 0;
};