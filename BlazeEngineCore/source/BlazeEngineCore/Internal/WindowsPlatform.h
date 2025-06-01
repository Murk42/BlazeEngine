#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"

namespace Blaze
{
	class String;
}

namespace Blaze::Windows
{	
	// Parameters:
	// error - usually the result of GetLastError()
	// recursionIndex - for internal use
	String GetErrorString(uint32 error, uint recursionIndex = 0);	
}