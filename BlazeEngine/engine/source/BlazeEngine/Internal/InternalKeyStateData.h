#pragma once
#include "BlazeEngine/Input/Input.h"

namespace Blaze
{
	struct InternalKeyStateData
	{		
		bool down;
		bool pressed;
		bool released;
		
		uint combo;
		double timePressed;
	};

	struct InternalMouseKeyStateData
	{
		bool down;
		bool pressed;
		bool released;
		
		uint combo;
		double timePressed;
	};
}