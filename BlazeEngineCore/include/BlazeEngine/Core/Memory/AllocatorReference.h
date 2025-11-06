#pragma once
#include "BlazeEngine/Core/Memory/Allocator.h"

namespace Blaze
{
	class AllocatorReference
	{
	public:
		AllocatorReference();
		AllocatorReference(const AllocatorReference& other);
		template<AllocatorType Allocator>
		AllocatorReference(Allocator& allocator);

		void* Allocate(uintMem bytes);
		void Free(void* ptr);

		AllocatorReference& operator=(const AllocatorReference& other);
	private:
		void* object;
		void* (*allocate)(void*, uintMem);
		void(*free)(void*, void*);
	};

	template<AllocatorType Allocator>
	inline AllocatorReference::AllocatorReference(Allocator& allocator)
	{
		object = &allocator;
		allocate = [](void* object, uintMem bytes) { ((Allocator*)object)->Allocate(bytes); };
		free = [](void* object, void* ptr) { ((Allocator*)object)->Free(ptr); };
	}
}