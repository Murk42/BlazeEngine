#include "pch.h"
#include "BlazeEngineCore/Memory/Allocator.h"
#include "BlazeEngineCore/Memory/MemoryManager.h"

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