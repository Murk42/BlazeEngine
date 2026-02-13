#include "pch.h"
#include "BlazeEngine/Core/Memory/MallocAllocator.h"
#include <memory>

namespace Blaze
{
	void* MallocAllocator::Allocate(uintMem byteCount)
	{
		return malloc(byteCount);
	}
	void MallocAllocator::Free(void* ptr)
	{
		free(ptr);
	}
}