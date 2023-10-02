#pragma once

namespace Blaze
{
	namespace Memory
	{		
		BLAZE_API void* Allocate(uint size);
		BLAZE_API void Free(void* ptr);		
		BLAZE_API void* Reallocate(uint size, void* old);
	}
}