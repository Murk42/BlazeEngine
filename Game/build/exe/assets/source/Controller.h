#pragma once
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

class Controller : public Behaviour<Controller>
{
public:	
	Vec2f rot;	

	void Update();
};