#pragma once
#include "BlazeEngine/Core/Startup.h"

namespace Blaze
{
	Startup::BlazeLibrariesInitInfo::SDLInitInfo InitializeSDL();
	void TerminateSDL();
}