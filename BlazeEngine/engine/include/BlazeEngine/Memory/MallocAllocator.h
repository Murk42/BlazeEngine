#pragma once
#include <malloc.h>
#include <utility>
#include <type_traits>

namespace Blaze
{
	template<typename T>
	class MallocAllocator
	{
	public:
		using pointer = T*;
		using const_pointer = const T*;
		using void_pointer = void*;
		using const_count_pointer = const void*;
		using value_type = T;
		using size_type = size_t;
		using difference_type = size_t;

		MallocAllocator() { }
		MallocAllocator(const MallocAllocator&) { }
		MallocAllocator(MallocAllocator&&) noexcept { }

		T* allocate(size_t n)
		{
			return (T*)malloc(sizeof(T) * n);
		}
		void deallocate(T* ptr, size_t n)
		{
			free(ptr);
		}
		template<typename ... Args>
		void construct(T* ptr, Args ... args)
		{
			new (ptr) T(args...);
		}
		void destroy(T* ptr)
		{
			ptr->~T();
		}

		template<typename T2>
		constexpr bool operator==(const MallocAllocator<T2>&) { return std::is_same_v<T, T2>; }
		template<typename T2>
		constexpr bool operator!=(const MallocAllocator<T2>&) { return !std::is_same_v<T, T2>; }
	};
}