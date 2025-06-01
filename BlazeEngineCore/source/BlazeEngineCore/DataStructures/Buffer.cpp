#include "pch.h"
#include "BlazeEngineCore/DataStructures/Buffer.h"
#include "BlazeEngineCore/Memory/MemoryManager.h"
#include <cstring>

namespace Blaze
{
	Buffer::Buffer()
		: ptr(nullptr)
	{ 
	}	
	Buffer::Buffer(Buffer&& buffer) noexcept
		: ptr(buffer.ptr)
	{
		buffer.ptr = nullptr;		
	}
	Buffer::Buffer(uintMem size)
	{
		ptr = Memory::Allocate(size);
	}
	Buffer::~Buffer()
	{
		if (ptr != nullptr)
			Memory::Free(ptr);
	}
	void Buffer::Clear()
	{
		if (ptr != nullptr)
			Memory::Free(ptr);
		ptr = nullptr;
	}
	void* Buffer::Allocate(uintMem size)
	{	
		if (ptr != nullptr)
			Memory::Free(ptr);
		ptr = Memory::Allocate(size);
		return ptr;
	}		
	Buffer& Buffer::operator=(Buffer&& buffer) noexcept
	{		
		Memory::Free(ptr);
		ptr = buffer.ptr;		
		buffer.ptr = nullptr;		
		return *this;
	}	
}