#pragma once

namespace Blaze
{
#ifdef BLAZE_COMPILER_MSVC
#define BLAZE_ALLOCATOR_ATTRIBUTE [[msvc::no_unique_address]]
#else
#define BLAZE_ALLOCATOR_ATTRIBUTE [[no_unique_address]]
#endif
	
	class BLAZE_API AllocatorBase
	{
	public:
		static void* Allocate(uint bytes);
		static void Free(void* ptr);
		static void* Reallocate(uint bytes, void* old);
		template<typename T, typename ... Args> requires std::constructible_from<T, Args...>
		static T* Create(Args&& ... args);
		template<typename T>
		static void Destroy(T* ptr);
	};

	class BLAZE_API DefaultAllocator : public AllocatorBase
	{			
	};			

	template<typename T>
	concept IsAllocator = std::derived_from<T, AllocatorBase>;

	template<typename T>
	concept AllocatorType = IsAllocator<T>;		

	template<typename T>
	concept IsStaticAllocator = !requires(const T & allocator, uint bytes, void* pointer)
	{
		{ allocator.Allocate(bytes) } -> std::same_as<void*>;
		{ allocator.Free(pointer) };
	} && IsAllocator<T>;

	template<typename T>
	concept IsDynamicAllocator = requires(const T & allocator, uint bytes, void* pointer)
	{
		{ allocator.Allocate(bytes) } -> std::same_as<void*>;
		{ allocator.Free(pointer) };
	} && IsAllocator<T>;	
		
	template<typename T, typename ... Args> requires std::constructible_from<T, Args...>
	inline T* AllocatorBase::Create(Args&& ... args)
	{
		T* ptr = (T*)Allocate(sizeof(T));
		std::construct_at(ptr, std::forward<Args>(args)...);
		return ptr;
	}	
	template<typename T>
	inline void AllocatorBase::Destroy(T* ptr)
	{
		if (ptr != nullptr)
		{
			std::destroy_at(ptr);
			Free(ptr);
		}
	}
}