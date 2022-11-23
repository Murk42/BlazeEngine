#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include <type_traits>
#include <cstring>
#include <memory>
#include <utility>

namespace Blaze
{
	template<typename T>
	class ArrayView;

	template<typename T, typename Allocator = std::allocator<T>>
	class Array
	{
		T* ptr;
		size_t count;

		static void CopyConstruct(T* dst, T* src, size_t count)
		{
			Allocator a;
			if (std::is_trivially_copyable_v<T>)
				memcpy(dst, src, sizeof(T) * count);
			else for (int i = 0; i < count; ++i)
				new (dst + i) T(src[i]);
		}
		static void MoveConstruct(T* dst, T* src, size_t count)
		{
			Allocator a;
			if (std::is_trivially_copyable_v<T>)
				memcpy(dst, src, sizeof(T) * count);
			else for (int i = 0; i < count; ++i)
				new (dst + i) T(std::move(src[i]));
		}
		static void CopyAssign(T* dst, T* src, size_t count)
		{
			if (std::is_trivially_copyable_v<T>)
				memcpy(dst, src, sizeof(T) * count);
			else for (int i = 0; i < count; ++i)
				dst[i] = src[i];
		}
	public:
		using value_type = T;

		Array()
			: ptr(nullptr), count(0)
		{
		}
		Array(const Array& arr)
		{
			Allocator a;
			
			ptr = a.allocate(arr.count);
			count = arr.count;
			CopyConstruct(ptr, arr.ptr, count);
		}
		Array(Array&& arr)
		{
			ptr = arr.ptr;
			count = arr.count;

			arr.ptr = nullptr;
			arr.count = 0;
		}
		Array(size_t count)
			: count(count)
		{
			Allocator a;

			ptr = a.allocate(count);
			for (int i = 0; i < count; ++i)
				new (ptr + i) T();
		}
		Array(const ArrayView<T>& arr);

		~Array()
		{
			Clear();
		}

		void Resize(size_t newCount)
		{
			if (newCount == 0)
			{
				ptr = nullptr;
				count = 0;
				return;
			}

			Allocator a;
			T* old = ptr;
			ptr = a.allocate(newCount);
			
			size_t smaller = std::min(newCount, count);
			MoveConstruct(ptr, old, smaller);

			for (int i = smaller; i < newCount; ++i)
				new (ptr + i) T();

			for (int i = 0; i < count; ++i)
				old[i].~T();

			a.deallocate(old, count);

			count = newCount;
		}
		void Clear()
		{
			Allocator a;

			for (int i = 0; i < count; ++i)
				ptr[i].~T();

			a.deallocate(ptr, count);
			ptr = nullptr;
			count = 0;
		}
		bool Empty() const { return count == 0; }

		T* Ptr() { return ptr; }
		const T* Ptr() const { return ptr; }
		size_t Count() const { return count; }

		T& operator[](size_t i)
		{
			if (i >= count)
				throw std::exception("index out of bounds");

			return ptr[i];
		}
		const T& operator[](size_t i) const
		{
			if (i >= count)
				throw std::exception("index out of bounds");

			return ptr[i];
		}

		T* begin() const { return ptr; }
		T* end() const { return ptr + count; }

		T& Last()
		{ 
			if (count == 0)
				throw std::exception("Array is empty");

			return ptr[count - 1]; 
		}
		const T& Last() const
		{
			if (count == 0)
				throw std::exception("Array is empty");

			return ptr[count - 1];
		}

		Array& operator=(const Array& arr)
		{
			Allocator a;
			for (int i = 0; i < count; ++i)
				ptr[i].~T();

			a.deallocate(ptr, count);

			ptr = a.allocate(arr.count);

			count = arr.count;

			CopyConstruct(ptr, arr.ptr, count);
			return *this;
		}
		Array& operator=(Array&& arr) noexcept
		{
			ptr = arr.ptr;
			count = arr.count;

			arr.ptr = nullptr;
			arr.count = 0;

			return *this;
		}
		Array& operator=(const ArrayView<T>& arr);
	};
}