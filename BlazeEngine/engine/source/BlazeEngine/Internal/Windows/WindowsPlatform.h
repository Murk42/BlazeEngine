#pragma once
#include <Windows.h>

namespace Blaze::Windows
{	
	// Parameters:
	// error - usually the result of GetLastError()
	// recursionIndex - for internal use
	String GetErrorString(DWORD error, uint recursionIndex = 0);	
}