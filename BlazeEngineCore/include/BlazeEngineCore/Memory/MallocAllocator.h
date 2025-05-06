#pragma once

namespace Blaze
{	
	class BLAZE_CORE_API MallocAllocator
	{
	public:				
		static void* Allocate(uintMem byteCount);
		static void Free(void* ptr);
	};
}