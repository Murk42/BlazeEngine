#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"
#include <concepts>

namespace Blaze
{
#define BLAZE_ALLOCATOR_ATTRIBUTE [[no_unique_address]]
		
	class BLAZE_CORE_API DefaultAllocator
	{	
	public:
		static void* Allocate(uintMem bytes);
		static void Free(void* ptr);
		static void* Reallocate(uintMem bytes, void* old);
	};			

	template<typename T>
	concept IsAllocator = requires (T& allocator, uintMem bytes, void* pointer) {
		{ allocator.Allocate(bytes) } -> std::same_as<void*>;
		{ allocator.Free(pointer) };
	};

	template<typename T>
	concept AllocatorType = IsAllocator<T>;			
}