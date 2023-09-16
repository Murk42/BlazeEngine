#include "UnorderedArray.h"
#pragma once

namespace Blaze
{
	template<typename T, AllocatorType Allocator>
	inline UnorderedArray<T, Allocator>::UnorderedArray()
		: ptr(nullptr), count(0), reserved(0)
	{
	}
	template<typename T, AllocatorType Allocator>
	inline UnorderedArray<T, Allocator>::UnorderedArray(const UnorderedArray& arr)
		: ptr(nullptr), count(0), reserved(0)
	{
		CopyUnsafe(arr.ptr, arr.count);
	}
	template<typename T, AllocatorType Allocator>
	inline UnorderedArray<T, Allocator>::UnorderedArray(UnorderedArray&& arr) noexcept
		: ptr(arr.ptr), count(arr.count), reserved(arr.reserved)
	{
		arr.ptr = nullptr;
		arr.count = 0;
		arr.reserved = 0;
	}
	template<typename T, AllocatorType Allocator>
	template<typename ... Args> requires std::constructible_from<T, Args...>
	inline UnorderedArray<T, Allocator>::UnorderedArray(uint count, const Args& ... args)
		: ptr(nullptr), count(count), reserved(count)
	{
		Resize(count, std::move(args)...);
	}
	template<typename T, AllocatorType Allocator>
	template<uint S>
	inline UnorderedArray<T, Allocator>::UnorderedArray(const T(&arr)[S])
	{
		CopyUnsafe(arr, S);
	}
	template<typename T, AllocatorType Allocator>
	inline UnorderedArray<T, Allocator>::UnorderedArray(const T* ptr, uint count)
	{
		CopyUnsafe(ptr, count);
	}
	template<typename T, AllocatorType Allocator>
	inline UnorderedArray<T, Allocator>::UnorderedArray(const std::initializer_list<T>& arr)
	{
		CopyUnsafe(arr.begin(), arr.size());
	}
	template<typename T, AllocatorType Allocator>
	inline UnorderedArray<T, Allocator>::UnorderedArray(const ArrayView<std::remove_const_t<T>>& arr)
	{
		CopyUnsafe(arr.Ptr(), arr.Count());
	}
	template<typename T, AllocatorType Allocator>
	inline UnorderedArray<T, Allocator>::UnorderedArray(const ArrayView<const std::remove_const_t<T>>& arr)
	{
		CopyUnsafe(arr.Ptr(), arr.Count());
	}
	template<typename T, AllocatorType Allocator>
	inline UnorderedArray<T, Allocator>::~UnorderedArray()
	{
		Clear();
	}
	template<typename T, AllocatorType Allocator>
	inline void UnorderedArray<T, Allocator>::Clear()
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (iteratorCount > 0)
			Debug::Logger::LogWarning("Blaze Engine", "Clearing an array while some iterators are referencing it");
#endif
		allocator.Free(ptr);
		ptr = nullptr;
		count = 0;
		reserved = 0;
	}
	template<typename T, AllocatorType Allocator>
	inline bool UnorderedArray<T, Allocator>::Empty() const
	{
		return count == 0;
	}
	template<typename T, AllocatorType Allocator>
	inline uint UnorderedArray<T, Allocator>::Count() const
	{
		return count;
	}	
	template<typename T, AllocatorType Allocator>
	template<typename ...Args> requires std::constructible_from<T, Args...>
	inline UnorderedArray<T, Allocator>::Iterator UnorderedArray<T, Allocator>::Add(Args && ...args)
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (iteratorCount > 0)
			Debug::Logger::LogWarning("Blaze Engine", "Changing an unordered array while some iterators are referencing it");
#endif

		if (auto newPtr = ReallocateUnsafe(count + 1))
		{
			for (uint i = 0; i < count; ++i)
				std::construct_at(newPtr + i, std::move(ptr[i]));			

			std::construct_at(newPtr + count, std::forward<Args>(args)...);

			std::destroy_n(ptr, count);
			allocator.Free(ptr);
			ptr = newPtr;
		}
		else		
			std::construct_at(ptr + count, std::forward<Args>(args)...);

		++count;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		return Iterator(ptr + count - 1, this);
#else
		return Iterator(ptr + count - 1);
#endif
	}
	template<typename T, AllocatorType Allocator>
	inline void UnorderedArray<T, Allocator>::EraseAt(uint index)
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (iteratorCount > 0)
			Debug::Logger::LogWarning("Blaze Engine", "Changing an array while some iterators are referencing it");
#endif

#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (index >= count)
			Debug::Logger::LogFatal("Blaze Engine", "Trying to erase an element outside the array");
#endif

		if (auto newPtr = ReallocateUnsafe(count - 1))
		{
			for (uint i = 0; i < index; ++i)
				std::construct_at(newPtr + i, std::move(ptr[i]));

			for (uint i = index; i < count - 1; ++i)
				std::construct_at(newPtr + i, std::move(ptr[i + 1]));

			std::destroy_n(ptr, count);
			allocator.Free(ptr);
			ptr = newPtr;
		}
		else
		{
			ptr[index] = std::move(ptr[count - 1]);			
			std::destroy_at(ptr + count);
		}

		count--;
	}
	template<typename T, AllocatorType Allocator>
	inline void UnorderedArray<T, Allocator>::EraseAt(Iterator it)
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (it.IsNull())
			Debug::Logger::LogFatal("Iterator is null");
#endif
		EraseAt(it.ptr - ptr);
	}
	template<typename T, AllocatorType Allocator>
	inline void UnorderedArray<T, Allocator>::Append(const UnorderedArray& other)
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (iteratorCount > 0)
			Debug::Logger::LogWarning("Blaze Engine", "Changing an array while some iterators are referencing it");
#endif

		if (auto newPtr = ReallocateUnsafe(other.count + other.count))
		{
			for (uint i = 0; i < count; ++i)
				std::construct_at(newPtr[i], std::move(ptr[i]));

			std::destroy_n(ptr, count);
			allocator.Free(ptr);
			ptr = newPtr;
		}

		for (uint i = count; i < count + other.count; ++i)
			std::construct_at(ptr + i, other.ptr[i]);

		count += other.count;
	}
	template<typename T, AllocatorType Allocator>
	inline void UnorderedArray<T, Allocator>::Append(UnorderedArray&& other)
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (iteratorCount > 0)
			Debug::Logger::LogWarning("Blaze Engine", "Changing an array while some iterators are referencing it");
#endif

		if (auto newPtr = ReallocateUnsafe(other.count + other.count))
		{
			for (uint i = 0; i < count; ++i)
				std::construct_at(newPtr[i], std::move(ptr[i]));

			std::destroy_n(ptr, count);
			allocator.Free(ptr);
			ptr = newPtr;
		}

		for (uint i = count; i < count + other.count; ++i)
			std::construct_at(ptr + i, std::move(other.ptr[i]));

		std::destroy_n(other.ptr, other.count);

		other.allocator.Free(other.ptr);
		other.ptr = nullptr;
		other.count = 0;
		other.reserved = 0;

		count += other.count;
	}	
	template<typename T, AllocatorType Allocator>
	template<typename ...Args> requires std::constructible_from<T, Args...>
	inline void UnorderedArray<T, Allocator>::Resize(uint newCount, const Args & ...args)
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (iteratorCount > 0)
			Debug::Logger::LogWarning("Blaze Engine", "Changing an array while some iterators are referencing it");
#endif
		auto newPtr = ReallocateUnsafe(newCount);

		if (newCount < count)
		{
			if (newPtr != nullptr)
			{
				for (uint i = 0; i < newCount; ++i)
					std::construct_at(newPtr + i, std::move(ptr[i]));

				std::destroy_n(ptr, count);

				allocator.Free(ptr);
				ptr = newPtr;
			}
			else
				std::destroy_n(ptr + newCount, count - newCount);
		}
		else
		{
			if (newPtr != nullptr)
			{
				for (uint i = 0; i < count; ++i)
					std::construct_at(newPtr + i, std::move(ptr[i]));

				std::destroy_n(ptr, count);

				allocator.Free(ptr);
				ptr = newPtr;

				for (uint i = count; i < newCount; ++i)
					std::construct_at(newPtr + i, std::forward<Args>(args)...);
			}
			else
				for (uint i = count; i < newCount; ++i)
					std::construct_at(newPtr + i, std::forward<Args>(args)...);
		}

		count = newCount;
	}
	template<typename T, AllocatorType Allocator>
	inline T& UnorderedArray<T, Allocator>::operator[](uint i)
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (i >= count)
			Debug::Logger::LogFatal("Blaze Engine", "Invalid index");
#endif
		return ptr[i];
	}
	template<typename T, AllocatorType Allocator>
	inline const T& UnorderedArray<T, Allocator>::operator[](uint i) const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (i >= count)
			Debug::Logger::LogFatal("Blaze Engine", "Invalid index");
#endif
		return ptr[i];
	}
	template<typename T, AllocatorType Allocator>
	inline T* UnorderedArray<T, Allocator>::Ptr()
	{
		return ptr;
	}
	template<typename T, AllocatorType Allocator>
	inline const T* UnorderedArray<T, Allocator>::Ptr() const
	{
		return ptr;
	}
	template<typename T, AllocatorType Allocator>
	inline T& UnorderedArray<T, Allocator>::First()
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			Debug::Logger::LogFatal("Blaze Engine", "UnorderedArray is empty");
#endif

		return ptr[0];
	}
	template<typename T, AllocatorType Allocator>
	inline const T& UnorderedArray<T, Allocator>::First() const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			Debug::Logger::LogFatal("Blaze Engine", "UnorderedArray is empty");
#endif
		return ptr[0];
	}
	template<typename T, AllocatorType Allocator>
	inline T& UnorderedArray<T, Allocator>::Last()
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			Debug::Logger::LogFatal("Blaze Engine", "UnorderedArray is empty");
#endif
		return ptr[count - 1];
	}
	template<typename T, AllocatorType Allocator>
	inline const T& UnorderedArray<T, Allocator>::Last() const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			Debug::Logger::LogFatal("Blaze Engine", "UnorderedArray is empty");
#endif
		return ptr[count - 1];
	}
	template<typename T, AllocatorType Allocator>
	inline UnorderedArray<T, Allocator>::Iterator UnorderedArray<T, Allocator>::GetIterator(uint index)
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (index >= count)
			Debug::Logger::LogFatal("Blaze Engine", "Index out of range");
#endif		
		return Iterator(ptr + index);
	}
	template<typename T, AllocatorType Allocator>
	inline UnorderedArray<T, Allocator>::ConstIterator UnorderedArray<T, Allocator>::GetIterator(uint index) const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (index >= count)
			Debug::Logger::LogFatal("Blaze Engine", "Index out of range");
#endif		
		return ConstIterator(ptr + index);
	}
	template<typename T, AllocatorType Allocator>
	inline UnorderedArray<T, Allocator>::Iterator UnorderedArray<T, Allocator>::FirstIterator()
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		return Iterator(ptr, this);
#else
		return Iterator(ptr);
#endif				
	}
	template<typename T, AllocatorType Allocator>
	inline UnorderedArray<T, Allocator>::ConstIterator UnorderedArray<T, Allocator>::FirstIterator() const
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		return ConstIterator(ptr, this);
#else
		return ConstIterator(ptr);
#endif				
	}
	template<typename T, AllocatorType Allocator>
	inline UnorderedArray<T, Allocator>::Iterator UnorderedArray<T, Allocator>::LastIterator()
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		return Iterator(ptr + count - 1, this);
#else
		return Iterator(ptr + count - 1);
#endif				
	}
	template<typename T, AllocatorType Allocator>
	inline UnorderedArray<T, Allocator>::ConstIterator UnorderedArray<T, Allocator>::LastIterator() const
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		return ConstIterator(ptr + count - 1, this);
#else
		return ConstIterator(ptr + count - 1);
#endif				
	}
	template<typename T, AllocatorType Allocator>
	inline UnorderedArray<T, Allocator>::Iterator UnorderedArray<T, Allocator>::AheadIterator()
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		return Iterator(ptr - 1, this);
#else
		return Iterator(ptr - 1);
#endif
	}
	template<typename T, AllocatorType Allocator>
	inline UnorderedArray<T, Allocator>::ConstIterator UnorderedArray<T, Allocator>::AheadIterator() const
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		return ConstIterator(ptr - 1, this);
#else
		return ConstIterator(ptr - 1);
#endif
	}
	template<typename T, AllocatorType Allocator>
	inline UnorderedArray<T, Allocator>::Iterator UnorderedArray<T, Allocator>::BehindIterator()
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		return Iterator(ptr + count, this);
#else
		return Iterator(ptr + count);
#endif				
	}
	template<typename T, AllocatorType Allocator>
	inline UnorderedArray<T, Allocator>::ConstIterator UnorderedArray<T, Allocator>::BehindIterator() const
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		return ConstIterator(ptr + count, this);
#else
		return ConstIterator(ptr + count);
#endif				
	}	
	template<typename T, AllocatorType Allocator>
	inline UnorderedArray<T, Allocator>::operator ArrayView<std::remove_const_t<T>>()
	{
		return UnorderedArrayView(ptr, count);
	}
	template<typename T, AllocatorType Allocator>
	inline UnorderedArray<T, Allocator>::operator ArrayView<const std::remove_const_t<T>>()
	{
		return UnorderedArrayView(ptr, count);
	}	
	template<typename T, AllocatorType Allocator>
	inline UnorderedArray<T, Allocator>& UnorderedArray<T, Allocator>::operator=(const ArrayView<std::remove_const_t<T>>& other)
	{
		std::destroy_n(ptr, count);

		CopyUnsafe(other.Ptr(), other.Count());

		return *this;
	}	
	template<typename T, AllocatorType Allocator>
	inline UnorderedArray<T, Allocator>& UnorderedArray<T, Allocator>::operator=(const ArrayView<const std::remove_const_t<T>>& other)
	{
		std::destroy_n(ptr, count);

		CopyUnsafe(other.Ptr(), other.Count());

		return *this;
	}
	template<typename T, AllocatorType Allocator>
	inline UnorderedArray<T, Allocator>& UnorderedArray<T, Allocator>::operator=(const UnorderedArray& other)
	{
		std::destroy_n(ptr, count);

		CopyUnsafe(other.ptr, other.count);

		return *this;
	}

	template<typename T, AllocatorType Allocator>
	inline UnorderedArray<T, Allocator>& UnorderedArray<T, Allocator>::operator=(UnorderedArray&& other) noexcept
	{
		ptr = other.ptr;
		count = other.count;
		reserved = other.reserved;
		other.ptr = nullptr;
		other.count = 0;
		other.reserved = 0;

		return *this;
	}
	template<typename T, AllocatorType Allocator>
	inline void UnorderedArray<T, Allocator>::CopyUnsafe(const T* src, uint count)
	{
		if (auto newPtr = ReallocateUnsafe(count))
		{
			allocator.Free(ptr);

			ptr = newPtr;
		}

		for (uint i = 0; i < count; ++i)
			std::construct_at(ptr + i, src[i]);

		this->count = count;
	}

	template<typename T, AllocatorType Allocator>
	inline T* UnorderedArray<T, Allocator>::ReallocateUnsafe(uint newCount)
	{
		if (reserved == 0)
		{
			reserved = newCount;
		}
		else if (newCount > reserved)
		{
			reserved *= 2;

			while (newCount > reserved)
				reserved *= 2;
		}
		else if (newCount * 4 <= count)
		{
			if (newCount == 0)
				return nullptr;

			reserved = (reserved + 3) / 4;

			while (newCount * 4 <= reserved)
				reserved = (reserved + 3) / 4;
		}
		else
			return nullptr;

		return (T*)Memory::Allocate(reserved * sizeof(T));
	}

	template<typename T, AllocatorType Allocator>
	UnorderedArray<T, Allocator>::Iterator begin(UnorderedArray<T, Allocator>& arr)
	{
		return arr.FirstIterator();
	}
	template<typename T, AllocatorType Allocator>
	UnorderedArray<T, Allocator>::ConstIterator begin(const UnorderedArray<T, Allocator>& arr)
	{
		return arr.FirstIterator();
	}
	template<typename T, AllocatorType Allocator>
	UnorderedArray<T, Allocator>::Iterator end(UnorderedArray<T, Allocator>& arr)
	{
		return arr.BehindIterator();
	}
	template<typename T, AllocatorType Allocator>
	UnorderedArray<T, Allocator>::ConstIterator end(const UnorderedArray<T, Allocator>& arr)
	{
		return arr.BehindIterator();
	}
}