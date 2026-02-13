#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/Type/TypeTraits.h"

namespace Blaze
{
#define BLAZE_ALLOCATOR_ATTRIBUTE [[no_unique_address]]

	class BLAZE_API DefaultAllocator
	{
	public:
		static void* Allocate(uintMem bytes);
		static void Free(void* ptr);
		static void* Reallocate(uintMem bytes, void* old);
	};

	template<typename T>
	concept IsAllocator = requires (T& allocator, uintMem bytes, void* pointer) {
		{ allocator.Allocate(bytes) } -> SameAs<void*>;
		{ allocator.Free(pointer) };
	};

	template<typename T>
	concept AllocatorType = IsAllocator<T>;
}