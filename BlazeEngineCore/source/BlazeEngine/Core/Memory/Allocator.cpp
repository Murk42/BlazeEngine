#include "pch.h"
#include "BlazeEngine/Core/Memory/Allocator.h"
#include "BlazeEngine/Core/Memory/MemoryManager.h"

namespace Blaze
{
	void* DefaultAllocator::Allocate(uintMem bytes)
	{
		return Memory::Allocate(bytes);
	}
	void DefaultAllocator::Free(void* ptr)
	{
		Memory::Free(ptr);
	}
	void* DefaultAllocator::Reallocate(uintMem bytes, void* old)
	{
		return Memory::Reallocate(bytes, old);
	}
}