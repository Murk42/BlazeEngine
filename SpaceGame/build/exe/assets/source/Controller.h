#pragma once
#define BLAZE_ONLY_CORE
#include "BlazeEngine/BlazeEngine.h"
using namespace Blaze;

#include "Transform.h"

class Controller
{
public:
	Transform* tran;
	bool active;

	Controller(Transform* tran, bool active = true)
		: tran(tran), active(active)
	{

	}

	virtual void Update()
	{

	}
};
