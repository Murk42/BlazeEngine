#include "pch.h"
#include "BlazeEngine/Core/Memory/AllocatorReference.h"

namespace Blaze
{
	AllocatorReference::AllocatorReference()
		: object(nullptr), free(nullptr), allocate(nullptr)
	{

	}
	AllocatorReference::AllocatorReference(const AllocatorReference& other)
		: object(other.object), free(other.free), allocate(other.allocate)
	{
	}
	void* AllocatorReference::Allocate(uintMem bytes)
	{
		if (object == nullptr)
			BLAZE_LOG_FATAL("AllocatorReference not initialized");

		return allocate(object, bytes);
	}
	void AllocatorReference::Free(void* ptr)
	{
		if (object == nullptr)
			BLAZE_LOG_FATAL("AllocatorReference not initialized");

		free(object, ptr);
	}
	AllocatorReference& AllocatorReference::operator=(const AllocatorReference& other)
	{
		object = other.object;
		allocate = other.allocate;
		free = other.free;
		return *this;
	}
}