#include "BlazeEngine/Memory/MallocAllocator.h"

namespace Blaze
{
	void* MallocAllocator::Allocate(uint byteCount)
	{
		return malloc(byteCount);
	}
	void MallocAllocator::Free(void* ptr)
	{
		free(ptr);
	}
}