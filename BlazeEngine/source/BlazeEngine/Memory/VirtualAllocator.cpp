#include "pch.h"
#include "BlazeEngine/Memory/VirtualAllocator.h"

namespace Blaze
{
	VirtualAllocator::VirtualAllocator()
		: implementation(nullptr)
	{

	}

	VirtualAllocator::VirtualAllocator(VirtualAllocator&& other) noexcept
		: implementation(other.implementation)
	{		
		other.implementation = nullptr;
	}

	void* VirtualAllocator::Allocate(uint bytes)
	{
		if (implementation != nullptr)
			return implementation->Allocate(bytes);
		return DefaultAllocator::Allocate(bytes);
	}
	void VirtualAllocator::Free(void* ptr)
	{
		if (implementation != nullptr)
			implementation->Free(ptr);
		DefaultAllocator::Free(ptr);
	}
	void* VirtualAllocator::Reallocate(uint bytes, void* old)
	{
		if (implementation != nullptr)
			return implementation->Reallocate(bytes, old);
		return DefaultAllocator::Reallocate(bytes, old);
	}	
}