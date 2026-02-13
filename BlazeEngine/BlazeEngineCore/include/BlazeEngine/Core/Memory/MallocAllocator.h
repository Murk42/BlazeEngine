#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"

namespace Blaze
{	
	class BLAZE_API MallocAllocator
	{
	public:				
		static void* Allocate(uintMem byteCount);
		static void Free(void* ptr);
	};
}