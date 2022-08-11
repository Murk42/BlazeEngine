#pragma once
#include "BlazeEngine/Core/Startup.h"

namespace Blaze
{	
	Startup::BlazeLibrariesInitInfo InitializeLibraries();
	void TerminateLibraries();
}