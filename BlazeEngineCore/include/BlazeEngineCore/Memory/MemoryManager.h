#pragma once

namespace Blaze
{
	namespace Memory
	{		
		BLAZE_CORE_API void* Allocate(uintMem size);
		BLAZE_CORE_API void Free(void* ptr);		
		BLAZE_CORE_API void* Reallocate(uintMem size, void* old);
	}
}