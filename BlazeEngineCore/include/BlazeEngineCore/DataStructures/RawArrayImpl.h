#pragma once
#include "BlazeEngineCore/DataStructures/RawArray.h"
#include "BlazeEngineCore/DataStructures/Array.h"
#include "BlazeEngineCore/DataStructures/ArrayView.h"
#include "BLazeEngineCore/Debug/Logger.h"


namespace Blaze
{
	template<typename T, AllocatorType Allocator> requires std::is_trivially_destructible_v<T>
	inline RawArray<T, Allocator>::RawArray()
		: ptr(nullptr)
	{
	}
	template<typename T, AllocatorType Allocator> requires std::is_trivially_destructible_v<T>
	inline RawArray<T, Allocator>::RawArray(RawArray&& other) noexcept
		: ptr(other.ptr)
	{
		other.ptr = nullptr;
	}
	template<typename T, AllocatorType Allocator> requires std::is_trivially_destructible_v<T>
	inline RawArray<T, Allocator>::RawArray(Iterator begin, Iterator end) requires std::copy_constructible<StoredType>
	{
		ptr = (T*)allocator.Allocate(sizeof(T) * (begin - end));

		uintMem i = 0;
		for (auto it = begin; it != end; ++it, ++i)
			std::construct_at(ptr + i, *it);
	}
	template<typename T, AllocatorType Allocator> requires std::is_trivially_destructible_v<T>
	template<typename F> requires std::invocable<F, typename RawArray<T, Allocator>::template StoredType*, uintMem>
	inline RawArray<T, Allocator>::RawArray(uintMem count, const F& constructFunction)
	{
		ptr = (T*)allocator.Allocate(sizeof(T) * count);
		
		for (uintMem i = 0; i < count; ++i)
			constructFunction(ptr, i);
	}
	template<typename T, AllocatorType Allocator> requires std::is_trivially_destructible_v<T>
	template<typename ...Args> requires std::constructible_from<typename RawArray<T, Allocator>::template StoredType, Args...>
	inline RawArray<T, Allocator>::RawArray(uintMem count, Args && ...args)
	{
		ptr = (T*)allocator.Allocate(sizeof(T) * count);

		for (uintMem i = 0; i < count; ++i)
			std::construct_at(ptr + i, std::forward<Args>(args)...);
	}
	template<typename T, AllocatorType Allocator> requires std::is_trivially_destructible_v<T>
	template<typename T2, uintMem S> requires std::constructible_from<typename RawArray<T, Allocator>::template StoredType, const T2&>
	inline RawArray<T, Allocator>::RawArray(const T2(&arr)[S])
	{
		ptr = (T*)allocator.Allocate(sizeof(T) * S);

		for (uintMem i = 0; i < S; ++i)
			std::construct_at(ptr + i, arr[i]);
	}
	template<typename T, AllocatorType Allocator> requires std::is_trivially_destructible_v<T>
	template<typename T2> requires std::constructible_from<typename RawArray<T, Allocator>::template StoredType, const T2&>
	inline RawArray<T, Allocator>::RawArray(const std::initializer_list<T2>& arr)
	{
		ptr = (T*)allocator.Allocate(sizeof(T) * arr.size());

		for (uintMem i = 0; i < arr.size(); ++i)
			std::construct_at(ptr + i, arr[i]);
	}
	template<typename T, AllocatorType Allocator> requires std::is_trivially_destructible_v<T>
	template<typename T2> requires std::constructible_from<typename RawArray<T, Allocator>::template StoredType, const T2&>
	inline RawArray<T, Allocator>::RawArray(const T2* src, uintMem count)
	{
		ptr = (T*)allocator.Allocate(sizeof(T) * count);

		for (uintMem i = 0; i < count; ++i)
			std::construct_at(ptr + i, src[i]);
	}
	template<typename T, AllocatorType Allocator> requires std::is_trivially_destructible_v<T>
	template<typename T2, AllocatorType Allocator2> requires std::constructible_from<typename Array<T, Allocator>::template StoredType, const T2&>
	inline RawArray<T, Allocator>::RawArray(const Array<T2, Allocator2>& other)
	{
		ptr = (T*)allocator.Allocate(sizeof(T) * other.Count());

		for (uintMem i = 0; i < other.Count(); ++i)
			std::construct_at(ptr + i, other[i]);
	}
	template<typename T, AllocatorType Allocator> requires std::is_trivially_destructible_v<T>
	template<typename T2> requires std::constructible_from<typename Array<T, Allocator>::template StoredType, const T2&>
	inline RawArray<T, Allocator>::RawArray(const ArrayView<T2>& other)
	{
		ptr = (T*)allocator.Allocate(sizeof(T) * other.Count());

		for (uintMem i = 0; i < other.Count(); ++i)
			std::construct_at(ptr + i, other[i]);
	}
	template<typename T, AllocatorType Allocator> requires std::is_trivially_destructible_v<T>
	inline RawArray<T, Allocator>::~RawArray()
	{
		allocator.Free(ptr);
	}
	template<typename T, AllocatorType Allocator> requires std::is_trivially_destructible_v<T>
	inline void RawArray<T, Allocator>::Clear()
	{
		allocator.Free(ptr);
		ptr = nullptr;
	}
	template<typename T, AllocatorType Allocator> requires std::is_trivially_destructible_v<T>
	inline bool RawArray<T, Allocator>::Empty() const
	{
		return ptr == nullptr;
	}
	template<typename T, AllocatorType Allocator> requires std::is_trivially_destructible_v<T>
	template<typename ... Args> requires std::constructible_from<typename RawArray<T, Allocator>::template StoredType, Args...>
	inline void RawArray<T, Allocator>::Reallocate(uintMem count, Args&& ... args)
	{
		Clear();

		ptr = (T*)allocator.Allocate(sizeof(T) * count);

		for (uintMem i = 0; i < count; ++i)
			std::construct_at(ptr + i, std::forward<Args>(args)...);
	}
	template<typename T, AllocatorType Allocator> requires std::is_trivially_destructible_v<T>
	inline T& RawArray<T, Allocator>::operator[](uintMem i)
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (ptr == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Accessing a empty RawArray");
#endif

		return ptr[i];		
	}
	template<typename T, AllocatorType Allocator> requires std::is_trivially_destructible_v<T>
	inline const T& RawArray<T, Allocator>::operator[](uintMem i) const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (ptr == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Accessing a empty RawArray");
#endif

		return ptr[i];
	}
	template<typename T, AllocatorType Allocator> requires std::is_trivially_destructible_v<T>
	inline auto RawArray<T, Allocator>::Ptr() -> StoredType*
	{
		return ptr;
	}
	template<typename T, AllocatorType Allocator> requires std::is_trivially_destructible_v<T>
	inline auto RawArray<T, Allocator>::Ptr() const -> const StoredType*
	{
		return ptr;
	}
	template<typename T, AllocatorType Allocator> requires std::is_trivially_destructible_v<T>
	inline T& RawArray<T, Allocator>::First()
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (ptr == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Accessing a empty RawArray");
#endif

		return *ptr;
	}
	template<typename T, AllocatorType Allocator> requires std::is_trivially_destructible_v<T>
	inline const T& RawArray<T, Allocator>::First() const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (ptr == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Accessing a empty RawArray");
#endif

		return *ptr;
	}
	template<typename T, AllocatorType Allocator> requires std::is_trivially_destructible_v<T>
	inline auto RawArray<T, Allocator>::GetIterator(uintMem index) -> Iterator
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (ptr == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Accessing a empty RawArray");
#endif

		return Iterator(ptr + index);
	}
	template<typename T, AllocatorType Allocator> requires std::is_trivially_destructible_v<T>
	inline auto RawArray<T, Allocator>::GetIterator(uintMem index) const -> ConstIterator
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (ptr == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Accessing a empty RawArray");
#endif

		return ConstIterator(ptr + index);		
	}
	template<typename T, AllocatorType Allocator> requires std::is_trivially_destructible_v<T>
	inline auto RawArray<T, Allocator>::FirstIterator() -> Iterator
	{
		return Iterator(ptr);
	}
	template<typename T, AllocatorType Allocator> requires std::is_trivially_destructible_v<T>
	inline auto RawArray<T, Allocator>::FirstIterator() const -> ConstIterator
	{
		return ConstIterator(ptr);
	}
	template<typename T, AllocatorType Allocator> requires std::is_trivially_destructible_v<T>
	inline auto RawArray<T, Allocator>::AheadIterator() -> Iterator
	{
		return Iterator(ptr - 1);
	}
	template<typename T, AllocatorType Allocator> requires std::is_trivially_destructible_v<T>
	inline auto RawArray<T, Allocator>::AheadIterator() const -> ConstIterator
	{
		return ConstIterator(ptr - 1);
	}
	template<typename T, AllocatorType Allocator> requires std::is_trivially_destructible_v<T>
	inline auto RawArray<T, Allocator>::operator=(RawArray&& other) noexcept -> RawArray&
	{
		Clear();
		ptr = other.ptr;
		other.ptr = nullptr;
		return *this;
	}
	template<typename T, AllocatorType Allocator> requires std::is_trivially_destructible_v<T>
	template<typename T2, AllocatorType Allocator2>  requires std::constructible_from<typename Array<T, Allocator>::template StoredType, const T2&>&& std::assignable_from<typename Array<T, Allocator>::template StoredType&, const T2&>
	inline auto RawArray<T, Allocator>::operator=(const Array<T2, Allocator2>& arr) -> RawArray&
	{
		allocator.Free(ptr);

		if (arr.Empty())
		{
			ptr = nullptr;
		}
		else
		{
			ptr = (T*)allocator.Allocate(sizeof(T) * arr.Count());

			for (uintMem i = 0; i < arr.Count(); ++i)
				std::construct_at(ptr + i, arr[i]);
		}

		return *this;
	}
	template<typename T, AllocatorType Allocator> requires std::is_trivially_destructible_v<T>
	template<typename T2>  requires std::constructible_from<typename Array<T, Allocator>::template StoredType, const T2&>&& std::assignable_from<typename Array<T, Allocator>::template StoredType&, const T2&>
	inline auto RawArray<T, Allocator>::operator=(const ArrayView<T2>& arr) -> RawArray&
	{
		allocator.Free(ptr);

		if (arr.Empty())
		{
			ptr = nullptr;
		}
		else
		{
			ptr = (T*)allocator.Allocate(sizeof(T) * arr.Count());

			for (uintMem i = 0; i < arr.Count(); ++i)
				std::construct_at(ptr + i, arr[i]);
		}

		return *this;
	}
}