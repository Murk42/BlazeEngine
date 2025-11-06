#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"

namespace Blaze
{
	namespace Memory
	{		
		BLAZE_API void* Allocate(uintMem size);
		BLAZE_API void Free(void* ptr);		
		BLAZE_API void* Reallocate(uintMem size, void* old);
	}
}