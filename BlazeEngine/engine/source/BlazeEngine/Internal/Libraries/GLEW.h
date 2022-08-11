#pragma once
#include "BlazeEngine/Core/Startup.h"

namespace Blaze
{
	Startup::BlazeLibrariesInitInfo::GLEWInitInfo InitializeGLEW();
	void TerminateGLEW();
}