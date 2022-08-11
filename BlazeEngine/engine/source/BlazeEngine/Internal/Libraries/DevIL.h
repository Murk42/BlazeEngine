#pragma once
#include "BlazeEngine/Core/Startup.h"

namespace Blaze
{
	Startup::BlazeLibrariesInitInfo::DevILInitInfo InitializeDevIL();
	void TerminateDevIL();
}