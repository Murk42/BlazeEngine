#include "pch.h"
#include "BlazeEngineCore/Memory/Allocator.h"
#include "BlazeEngineCore/Memory/MemoryManager.h"

namespace Blaze
{
	void* AllocatorBase::Allocate(uintMem bytes)
	{
		return Memory::Allocate(bytes);
	}
	void AllocatorBase::Free(void* ptr)
	{
		Memory::Free(ptr);
	}
	void* AllocatorBase::Reallocate(uintMem bytes, void* old)
	{
		return Memory::Reallocate(bytes, old);
	}
}