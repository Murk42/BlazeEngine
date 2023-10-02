#include "pch.h"
#include "BlazeEngine/Memory/Allocator.h"

namespace Blaze
{
	void* AllocatorBase::Allocate(uint bytes)
	{
		return Memory::Allocate(bytes);
	}
	void AllocatorBase::Free(void* ptr)
	{
		Memory::Free(ptr);
	}
	void* AllocatorBase::Reallocate(uint bytes, void* old)
	{
		return Memory::Reallocate(bytes, old);
	}
}