#pragma once
#include "BlazeEngine/Core/Startup.h"

namespace Blaze
{
	Startup::BlazeLibrariesInitInfo::FreeTypeInitInfo InitializeFreeType();
	void TerminateFreeType();
}