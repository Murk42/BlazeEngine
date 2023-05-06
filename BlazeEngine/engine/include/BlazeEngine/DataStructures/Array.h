#pragma once
#include <type_traits>
#include <utility>
#include <memory>

namespace Blaze
{
	template<typename T>
	class ArrayView;

	template<typename T, typename Allocator = std::allocator<T>>
	class Array
	{
		T* ptr;
		uint count;

		static void CopyConstruct(T* dst, T* src, uint count)
		{
			Allocator a;
			if (std::is_trivially_copyable_v<T>)
				memcpy(dst, src, sizeof(T) * count);
			else for (int i = 0; i < count; ++i)
				new (dst + i) T(src[i]);
		}
		static void MoveConstruct(T* dst, T* src, uint count)
		{
			Allocator a;
			if (std::is_trivially_copyable_v<T>)
				memcpy(dst, src, sizeof(T) * count);
			else for (int i = 0; i < count; ++i)
				new (dst + i) T(std::move(src[i]));
		}
		static void CopyAssign(T* dst, T* src, uint count)
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
		Array(Array&& arr) noexcept
		{
			ptr = arr.ptr;
			count = arr.count;

			arr.ptr = nullptr;
			arr.count = 0;
		}
		Array(uint count)
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

		template<typename ... Args>
		void Resize(uint newCount, Args&& ... args)
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
			
			uint smaller = std::min(newCount, count);
			MoveConstruct(ptr, old, smaller);

			for (uint i = smaller; i < newCount; ++i)
				new (ptr + i) T(std::forward<Args>(args)...);

			for (int i = 0; i < count; ++i)
				old[i].~T();

			a.deallocate(old, count);

			count = newCount;
		}
		template<typename ... Args>
		T& Add(Args&& ... args)
		{
			uint oldCount = count;
			Resize(oldCount + 1, std::forward<Args>(args)...);
			return ptr[oldCount];
		}

		void Append(const Array& other)
		{
			Allocator a;
			size_t newCount = count + other.count;

			if (newCount == 0)
			{
				a.deallocate(ptr, count);
				ptr = nullptr;
				count = 0;
			}

			T* old = ptr;
			ptr = a.allocate(newCount);
			
			MoveConstruct(ptr, old, count);

			CopyConstruct(ptr + count, other.ptr, other.count);

			for (int i = 0; i < count; ++i)
				old[i].~T();

			a.deallocate(old, count);

			count = newCount;
		}
		void Append(Array&& other)
		{
			Allocator a;
			size_t newCount = count + other.count;

			if (newCount == 0)
			{
				a.deallocate(ptr, count);
				ptr = nullptr;
				count = 0;
				return;
			}

			T* old = ptr;
			ptr = a.allocate(newCount);

			MoveConstruct(ptr, old, count);

			MoveConstruct(ptr + count, other.ptr, other.count);

			for (int i = 0; i < count; ++i)
				old[i].~T();			

			a.deallocate(old, count);

			count = newCount;

			other.Clear();
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
		uint Count() const { return count; }

		T& operator[](uint i)
		{
			if (i >= count)
				throw std::exception("index out of bounds");

			return ptr[i];
		}
		const T& operator[](uint i) const
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