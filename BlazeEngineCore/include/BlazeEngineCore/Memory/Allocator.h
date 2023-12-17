#pragma once

namespace Blaze
{
#ifdef BLAZE_COMPILER_MSVC
#define BLAZE_ALLOCATOR_ATTRIBUTE [[msvc::no_unique_address]]
#else
#define BLAZE_ALLOCATOR_ATTRIBUTE [[no_unique_address]]
#endif
	
	class BLAZE_CORE_API AllocatorBase
	{
	public:
		static void* Allocate(uintMem bytes);
		static void Free(void* ptr);
		static void* Reallocate(uintMem bytes, void* old);		
	};

	class BLAZE_CORE_API DefaultAllocator : public AllocatorBase
	{			
	};			

	template<typename T>
	concept IsAllocator = std::derived_from<T, AllocatorBase>;

	template<typename T>
	concept AllocatorType = IsAllocator<T>;		

	template<typename T>
	concept IsStaticAllocator = !requires(const T & allocator, uintMem bytes, void* pointer)
	{
		{ allocator.Allocate(bytes) } -> std::same_as<void*>;
		{ allocator.Free(pointer) };
	} && IsAllocator<T>;

	template<typename T>
	concept IsDynamicAllocator = requires(const T & allocator, uintMem bytes, void* pointer)
	{
		{ allocator.Allocate(bytes) } -> std::same_as<void*>;
		{ allocator.Free(pointer) };
	} && IsAllocator<T>;	
}