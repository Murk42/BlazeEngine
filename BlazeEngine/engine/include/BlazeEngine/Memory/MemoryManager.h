#pragma once

namespace Blaze
{
	namespace Memory
	{		
		BLAZE_API void* Allocate(size_t size);
		BLAZE_API void Free(void* ptr);		
	}
}