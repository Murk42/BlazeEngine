#pragma once

namespace Blaze
{
	template<typename Array>
	inline ArrayIterator<Array>::ArrayIterator()
		: ptr(nullptr)
	{
	}
	template<typename Array>
	template<IsConvertibleToArrayIterator<ArrayIterator<Array>> T>
	inline ArrayIterator<Array>::ArrayIterator(const T& other)
		: ptr(other.ptr)
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		, arr(other.arr)
#endif
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (arr != nullptr)
			++arr->iteratorCount;
#endif
	}
	template<typename Array>
	inline ArrayIterator<Array>::~ArrayIterator()
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (arr != nullptr)
			--arr->iteratorCount;
#endif
	}
	template<typename Array>
	inline bool ArrayIterator<Array>::IsNull() const
	{
		return ptr == nullptr;
	}
	template<typename Array>
	inline ArrayIterator<Array>::ValueType& ArrayIterator<Array>::operator*() const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			Debug::Logger::LogFatal("Blaze Engine", "Dereferencing a null iterator");
#endif

		return *ptr;
	}
	template<typename Array>
	inline ArrayIterator<Array>::ValueType* ArrayIterator<Array>::operator->() const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			Debug::Logger::LogFatal("Blaze Engine", "Dereferencing a null iterator");
#endif

		return ptr;
	}
	template<typename Array>
	inline ArrayIterator<Array>& ArrayIterator<Array>::operator++()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			Debug::Logger::LogFatal("Blaze Engine", "Incrementing a null iterator");
#endif

		++ptr;
		return *this;
	}
	template<typename Array>
	inline ArrayIterator<Array> ArrayIterator<Array>::operator++(int)
	{
		ArrayIterator copy = *this;
		++copy;
		return copy;
	}
	template<typename Array>
	inline ArrayIterator<Array>& ArrayIterator<Array>::operator--()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			Debug::Logger::LogFatal("Blaze Engine", "Decrementing a null iterator");
#endif

		--ptr;
		return *this;
	}
	template<typename Array>
	inline ArrayIterator<Array> ArrayIterator<Array>::operator--(int)
	{
		ArrayIterator copy = *this;
		--copy;
		return copy;
	}
	template<typename Array>
	inline ArrayIterator<Array> ArrayIterator<Array>::operator+(int offset) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			Debug::Logger::LogFatal("Blaze Engine", "Adding to a null iterator");
#endif		
		return ArrayIterator(ptr + offset);
	}
	template<typename Array>
	inline ArrayIterator<Array> ArrayIterator<Array>::operator-(int offset) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			Debug::Logger::LogFatal("Blaze Engine", "Subtracting from a null iterator");
#endif		
		return ArrayIterator(ptr - offset);
	}
	template<typename Array>
	inline uintMem ArrayIterator<Array>::operator-(const ArrayIterator<Array>& other) const
	{
		return ptr - other.ptr;
	}
	template<typename Array>
	inline ArrayIterator<Array>& ArrayIterator<Array>::operator+=(int offset)
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			Debug::Logger::LogFatal("Blaze Engine", "Adding to a null iterator");
#endif		
		ptr += offset;

		return *this;
	}
	template<typename Array>
	inline ArrayIterator<Array>& ArrayIterator<Array>::operator-=(int offset)
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			Debug::Logger::LogFatal("Blaze Engine", "Subtracting from a null iterator");
#endif		
		ptr -= offset;

		return *this;
	}
	template<typename Array>
	template<IsComparableToArrayIterator<ArrayIterator<Array>> T>
	inline bool ArrayIterator<Array>::operator>(const T& i) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if ((ptr == nullptr) != (i.ptr == nullptr))
			Debug::Logger::LogFatal("Blaze Engine", "Comparing a null iterator");
#endif

		return ptr > i.ptr;
	}
	template<typename Array>
	template<IsComparableToArrayIterator<ArrayIterator<Array>> T>
	inline bool ArrayIterator<Array>::operator>=(const T& i) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if ((ptr == nullptr) != (i.ptr == nullptr))
			Debug::Logger::LogFatal("Blaze Engine", "Comparing a null iterator");
#endif

		return ptr >= i.ptr;
	}
	template<typename Array>
	template<IsComparableToArrayIterator<ArrayIterator<Array>> T>
	inline bool ArrayIterator<Array>::operator<(const T& i) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if ((ptr == nullptr) != (i.ptr == nullptr))
			Debug::Logger::LogFatal("Blaze Engine", "Comparing a null iterator");
#endif

		return ptr < i.ptr;
	}
	template<typename Array>
	template<IsComparableToArrayIterator<ArrayIterator<Array>> T>
	inline bool ArrayIterator<Array>::operator<=(const T& i) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if ((ptr == nullptr) != (i.ptr == nullptr))
			Debug::Logger::LogFatal("Blaze Engine", "Comparing a null iterator");
#endif

		return ptr <= i.ptr;
	}
	template<typename Array>
	template<IsComparableToArrayIterator<ArrayIterator<Array>> T>
	inline bool ArrayIterator<Array>::operator==(const T& i) const
	{
		return ptr == i.ptr;
	}
	template<typename Array>
	template<IsComparableToArrayIterator<ArrayIterator<Array>> T>
	inline bool ArrayIterator<Array>::operator!=(const T& i) const
	{
		return ptr != i.ptr;
	}
	template<typename Array>
	template<IsConvertibleToArrayIterator<ArrayIterator<Array>> T>
	inline ArrayIterator<Array>& ArrayIterator<Array>::operator=(const T& i)
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (arr != nullptr)
			--arr->iteratorCount;

		arr = i.arr;

		if (arr != nullptr)
			++arr->iteratorCount;
#endif
		ptr = i.ptr;

		return *this;
	}
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
	template<typename Array>
	inline ArrayIterator<Array>::ArrayIterator(ValueType* ptr, Array* arr)
		: ptr(ptr), arr(arr)
	{
		++arr->iteratorCount;
	}
#else
	template<typename Array>
	inline ArrayIterator<Array>::ArrayIterator(ValueType* ptr)
		: ptr(ptr)
	{
	}
#endif

	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::Array()
		: ptr(nullptr), count(0), reserved(0)
	{
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::Array(const Array& arr) requires std::is_copy_constructible_v<T>
		: ptr(nullptr), count(0), reserved(0)
	{ 
		CopyUnsafe(arr.ptr, arr.count);
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::Array(Array&& arr) noexcept
		: ptr(arr.ptr), count(arr.count), reserved(arr.reserved)
	{
		arr.ptr = nullptr;
		arr.count = 0;
		arr.reserved = 0;
	}
	template<typename T, AllocatorType Allocator>
	template<typename ... Args> requires std::constructible_from<T, Args...>
	inline Array<T, Allocator>::Array(uintMem count, const Args& ... args)
		: ptr(nullptr), count(count), reserved(count)
	{
		ptr = (T*)allocator.Allocate(sizeof(T) * count);
		
		for (uintMem i = 0; i < count; ++i)
			std::construct_at(ptr + i, args...);				
	}
	template<typename T, AllocatorType Allocator>
	template<typename F> requires std::invocable<F, T*, uintMem>
	inline Array<T, Allocator>::Array(uintMem count, const F& constructFunction)
		: ptr(nullptr), count(count), reserved(count)
	{
		ptr = (T*)allocator.Allocate(sizeof(T) * count);

		for (uintMem i = 0; i < count; ++i)
			constructFunction(ptr + i, i);
	}
	template<typename T, AllocatorType Allocator>
	template<uintMem S>
	inline Array<T, Allocator>::Array(const T (&arr)[S]) requires std::is_copy_constructible_v<T>
		: ptr(nullptr), count(0), reserved(0)
	{
		CopyUnsafe(arr, S);
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::Array(const T* ptr, uintMem count) requires std::is_copy_constructible_v<T>
		: ptr(nullptr), count(0), reserved(0)
	{
		CopyUnsafe(ptr, count);
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::Array(const std::initializer_list<T>& arr) requires std::is_copy_constructible_v<T>
		: ptr(nullptr), count(0), reserved(0)
	{
		CopyUnsafe(arr.begin(), arr.size());
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::Array(const ArrayView<std::remove_const_t<T>>& arr) requires std::is_copy_constructible_v<T>
		: ptr(nullptr), count(0), reserved(0)
	{
		CopyUnsafe(arr.Ptr(), arr.Count());
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::~Array()
	{
		Clear();
	}
	template<typename T, AllocatorType Allocator>
	inline void Array<T, Allocator>::Clear()
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (iteratorCount > 0)
			Debug::Logger::LogWarning("Blaze Engine", "Clearing an array while some iterators are referencing it");
#endif
		std::destroy_n(ptr, count);
		allocator.Free(ptr);
		ptr = nullptr;
		count = 0;
		reserved = 0;
	}
	template<typename T, AllocatorType Allocator>
	inline bool Array<T, Allocator>::Empty() const
	{
		return count == 0;
	}
	template<typename T, AllocatorType Allocator>
	inline uintMem Array<T, Allocator>::Count() const
	{
		return count;
	}
	template<typename T, AllocatorType Allocator>
	inline uintMem Array<T, Allocator>::ReservedCount() const
	{
		return reserved;
	}
	template<typename T, AllocatorType Allocator>
	template<typename ...Args> requires std::constructible_from<T, Args...>
	inline Array<T, Allocator>::Iterator Array<T, Allocator>::AddBack(Args && ...args) requires std::constructible_from<T, T&&>
	{
		if (auto newPtr = ReallocateUnsafe(count + 1))
		{
			for (uintMem i = 0; i < count; ++i)
				std::construct_at(newPtr + i, std::move(ptr[i]));

			std::construct_at(newPtr + count, std::forward<Args>(args)...);

			std::destroy_n(ptr, count);
			allocator.Free(ptr);
			ptr = newPtr;
		}
		else
		{
			std::construct_at(ptr + count, std::forward<Args>(args)...);
		}
		++count;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		return Iterator(ptr + count - 1, this);
#else
		return Iterator(ptr + count - 1);
#endif
	}
	template<typename T, AllocatorType Allocator>
	template<typename ...Args> requires std::constructible_from<T, Args...>
	inline Array<T, Allocator>::Iterator Array<T, Allocator>::TryAddBack(Args && ...args)
	{
		if (count < reserved)
		{
			std::construct_at(ptr + count, std::forward<Args>(args)...);
			++count;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		return Iterator(ptr + count - 1, this);
#else
		return Iterator(ptr + count - 1);
#endif
		}
		else
			return Iterator();

	}
	template<typename T, AllocatorType Allocator>
	template<typename ...Args> requires std::constructible_from<T, Args...>
	inline Array<T, Allocator>::Iterator Array<T, Allocator>::AddAt(uintMem index, Args && ...args) requires std::constructible_from<T, T&&>
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (index > count)
			Debug::Logger::LogFatal("Blaze Engine", "Trying to add an element outside the array");
#endif

		if (auto newPtr = ReallocateUnsafe(count + 1))
		{
			for (uintMem i = 0; i < index; ++i)
				std::construct_at(newPtr + i, std::move(ptr[i]));

			for (uintMem i = index; i < count; ++i)
				std::construct_at(newPtr + i + 1, std::move(ptr[i]));

			std::construct_at(newPtr + index, std::forward<Args>(args)...);

			std::destroy_n(ptr, count);
			allocator.Free(ptr);
			ptr = newPtr;
		}
		else
		{
			for (uintMem i = count; i > index; --i)
				ptr[i] = std::move(ptr[i - 1]);

			std::construct_at(ptr + index, std::forward<Args>(args)...);
		}
		++count;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		return Iterator(ptr + index, this);
#else
		return Iterator(ptr + index);
#endif
	}
	template<typename T, AllocatorType Allocator>
	template<typename ...Args> requires std::constructible_from<T, Args...>
	inline Array<T, Allocator>::Iterator Array<T, Allocator>::AddAt(Iterator it, Args && ...args) requires std::constructible_from<T, T&&>
	{
		return AddAt(it.ptr - ptr, std::forward<Args>(args)...);
	}
	template<typename T, AllocatorType Allocator>	
	inline void Array<T, Allocator>::Insert(Iterator it, ArrayView<T> array)  requires std::constructible_from<T, T&&>
	{		
		Insert(it.ptr - ptr, array);
	}
	template<typename T, AllocatorType Allocator>
	inline void Array<T, Allocator>::Insert(uintMem index, ArrayView<T> array)  requires std::constructible_from<T, T&&>
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (index > count)
			Debug::Logger::LogFatal("Blaze Engine", "Trying to add an element outside the array");
#endif

		if (auto newPtr = ReallocateUnsafe(count + array.Count()))
		{
			uintMem i = 0;
			for (; i < index; ++i)
				std::construct_at(newPtr + i, std::move(ptr[i]));

			for (; i < index + array.Count(); ++i)
				std::construct_at(newPtr + i, std::move(array[i - index]));

			for (; i < count + array.Count(); ++i)
				std::construct_at(newPtr + i, std::move(ptr[i - array.Count()]));

			std::destroy_n(ptr, count);
			allocator.Free(ptr);
			ptr = newPtr;
		}
		else
		{
			for (uintMem i = 1; i < array.Count() + 1; ++i)
				ptr[count + array.Count() - i] = std::move(ptr[count - i]);

			for (uintMem i = index; i < index + array.Count(); ++i)
				std::construct_at(ptr + i, std::move(array[i - index]));			
		}

		count += array.Count();
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator> Array<T, Allocator>::Split(Iterator it)
	{
		return Split(it.ptr - ptr);
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator> Array<T, Allocator>::Split(uintMem index)
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (index > count)
			Debug::Logger::LogFatal("Blaze Engine", "Trying to add an element outside the array");
#endif

		auto oldPtr = ptr;
		auto oldCount = count;

		if (auto newPtr = ReallocateUnsafe(index))
		{
			for (uintMem i = 0; i < index; ++i)
				std::construct_at(newPtr + i, std::move(ptr[i]));

			ptr = newPtr;
		}

		count = index;
			
		return Array<T, Allocator>(oldCount - count, [oldPtr, index](T* ptr, uintMem i) {
			std::construct_at(ptr, std::move(*(oldPtr + index + i)));
			});
	}
	template<typename T, AllocatorType Allocator>
	inline void Array<T, Allocator>::EraseLast()
	{
		if (auto newPtr = ReallocateUnsafe(count - 1))
		{
			for (uintMem i = 0; i < count - 1; ++i)
				std::construct_at(newPtr + i, std::move(ptr[i]));

			std::destroy_n(ptr, count);
			allocator.Free(ptr);
			ptr = newPtr;
		}
		else
		{
			std::destroy_at(ptr + count);
		}

		count--;
	}
	template<typename T, AllocatorType Allocator>
	inline void Array<T, Allocator>::EraseAt(uintMem index)
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (index >= count)
			Debug::Logger::LogFatal("Blaze Engine", "Trying to erase an element outside the array");
#endif

		if (auto newPtr = ReallocateUnsafe(count - 1))
		{
			for (uintMem i = 0; i < index; ++i)
				std::construct_at(newPtr + i, std::move(ptr[i]));

			for (uintMem i = index; i < count - 1; ++i)
				std::construct_at(newPtr + i, std::move(ptr[i + 1]));

			std::destroy_n(ptr, count);
			allocator.Free(ptr);
			ptr = newPtr;
		}
		else
		{
			for (uintMem i = index; i < count - 1; ++i)
				ptr[i] = std::move(ptr[i + 1]);

			std::destroy_at(ptr + count - 1);
		}

		count--;
	}
	template<typename T, AllocatorType Allocator>
	inline void Array<T, Allocator>::EraseAt(Iterator it)
	{
		EraseAt(it.ptr - ptr);
	}
	template<typename T, AllocatorType Allocator>
	inline void Array<T, Allocator>::Append(const Array& other) requires std::constructible_from<T, const T&>
	{
		if (auto newPtr = ReallocateUnsafe(count + other.count))
		{
			for (uintMem i = 0; i < count; ++i)
				std::construct_at(newPtr + i, std::move(ptr[i]));

			std::destroy_n(ptr, count);
			allocator.Free(ptr);
			ptr = newPtr;
		}

		for (uintMem i = 0; i < other.count; ++i)
			std::construct_at(ptr + count + i, other.ptr[i]);

		count += other.count;
	}
	template<typename T, AllocatorType Allocator>
	inline void Array<T, Allocator>::Append(Array&& other) requires std::constructible_from<T, T&&>
	{
		if (auto newPtr = ReallocateUnsafe(count + other.count))
		{
			for (uintMem i = 0; i < count; ++i)
				std::construct_at(newPtr + i, std::move(ptr[i]));

			std::destroy_n(ptr, count);
			allocator.Free(ptr);
			ptr = newPtr;
		}

		for (uintMem i = 0; i < other.count; ++i)
			std::construct_at(ptr + i + count, std::move(other.ptr[i]));

		std::destroy_n(other.ptr, other.count);

		count += other.count;

		other.allocator.Free(other.ptr);
		other.ptr = nullptr;
		other.count = 0;
		other.reserved = 0;
	}
	template<typename T, AllocatorType Allocator>
	template<typename ...Args> requires std::constructible_from<T, Args...>
	inline void Array<T, Allocator>::Resize(uintMem newCount, Args&& ...args) requires std::constructible_from<T, T&&>
	{
		auto newPtr = ReallocateUnsafe(newCount);

		if (newCount < count)
		{
			if (newPtr != nullptr)
			{
				for (uintMem i = 0; i < newCount; ++i)
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
				for (uintMem i = 0; i < count; ++i)
					std::construct_at(newPtr + i, std::move(ptr[i]));

				std::destroy_n(ptr, count);

				allocator.Free(ptr);
				ptr = newPtr;				
			}			

			for (uintMem i = count; i < newCount; ++i)
				std::construct_at(ptr + i, std::forward<Args>(args)...);
		}

		count = newCount;
	}	
	template<typename T, AllocatorType Allocator>
	template<typename F> requires std::invocable<F, T*, uintMem>
	void Array<T, Allocator>::ResizeWithFunction(uintMem newCount, const F& constructFunction) requires std::constructible_from<T, T&&>
	{
		auto newPtr = ReallocateUnsafe(newCount);

		if (newCount < count)
		{
			if (newPtr != nullptr)
			{
				for (uintMem i = 0; i < newCount; ++i)
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
				for (uintMem i = 0; i < count; ++i)
					std::construct_at(newPtr + i, std::move(ptr[i]));

				std::destroy_n(ptr, count);

				allocator.Free(ptr);
				ptr = newPtr;
			}

			for (uintMem i = count; i < newCount; ++i)
				constructFunction(ptr + i, i);				
		}

		count = newCount;
	}	
	template<typename T, AllocatorType Allocator>
	void Array<T, Allocator>::ReserveExactly(uintMem reservedCount)
	{		
		if (auto newPtr = ReallocateUnsafe(reservedCount))
		{			
			std::destroy_n(ptr, count);
			allocator.Free(ptr);

			ptr = newPtr;
			count = 0;
		}
		else
		{
			std::destroy_n(ptr, count);
			count = 0;
		}
	}
	template<typename T, AllocatorType Allocator>	
	void Array<T, Allocator>::ReserveAdditional(uintMem reservedCount) requires std::constructible_from<T, T&&>
	{		
		if (auto newPtr = ReallocateUnsafe(count + reservedCount))
		{
			for (uintMem i = 0; i < count; ++i)
				std::construct_at(newPtr + i, std::move(ptr[i]));

			std::destroy_n(ptr, count);
			allocator.Free(ptr);

			ptr = newPtr;
		}		
	}
	template<typename T, AllocatorType Allocator>
	inline void Array<T, Allocator>::Truncate(uintMem newCount)
	{
		if (count < newCount)
			return;

		for (uintMem i = newCount; i < count; ++i)
			std::destroy_at(ptr + i);

		count = newCount;
	}
	template<typename T, AllocatorType Allocator>
	inline T& Array<T, Allocator>::operator[](uintMem i)
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (i >= count)
			Debug::Logger::LogFatal("Blaze Engine", "Invalid index");
#endif
		return ptr[i];
	}
	template<typename T, AllocatorType Allocator>
	inline const T& Array<T, Allocator>::operator[](uintMem i) const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (i >= count)
			Debug::Logger::LogFatal("Blaze Engine", "Invalid index");
#endif
		return ptr[i];
	}
	template<typename T, AllocatorType Allocator>
	inline T* Array<T, Allocator>::Ptr()
	{
		return ptr;
	}
	template<typename T, AllocatorType Allocator>
	inline const T* Array<T, Allocator>::Ptr() const
	{
		return ptr;
	}
	template<typename T, AllocatorType Allocator>
	inline T& Array<T, Allocator>::First()
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			Debug::Logger::LogFatal("Blaze Engine", "Array is empty");
#endif

		return ptr[0];
	}
	template<typename T, AllocatorType Allocator>
	inline const T& Array<T, Allocator>::First() const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			Debug::Logger::LogFatal("Blaze Engine", "Array is empty");
#endif
		return ptr[0];
	}
	template<typename T, AllocatorType Allocator>
	inline T& Array<T, Allocator>::Last()
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			Debug::Logger::LogFatal("Blaze Engine", "Array is empty");
#endif
		return ptr[count - 1];
	}
	template<typename T, AllocatorType Allocator>
	inline const T& Array<T, Allocator>::Last() const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			Debug::Logger::LogFatal("Blaze Engine", "Array is empty");
#endif
		return ptr[count - 1];
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::Iterator Array<T, Allocator>::GetIterator(uintMem index)
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (index >= count)
			Debug::Logger::LogFatal("Blaze Engine", "Index out of range");
#endif		
		return Iterator(ptr + index);
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::ConstIterator Array<T, Allocator>::GetIterator(uintMem index) const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (index >= count)
			Debug::Logger::LogFatal("Blaze Engine", "Index out of range");
#endif		
		return ConstIterator(ptr + index);
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::Iterator Array<T, Allocator>::FirstIterator()
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		return Iterator(ptr, this);
#else
		return Iterator(ptr);
#endif				
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::ConstIterator Array<T, Allocator>::FirstIterator() const
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		return ConstIterator(ptr, this);
#else
		return ConstIterator(ptr);
#endif				
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::Iterator Array<T, Allocator>::LastIterator()
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		return Iterator(ptr + count - 1, this);
#else
		return Iterator(ptr + count - 1);
#endif				
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::ConstIterator Array<T, Allocator>::LastIterator() const
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		return ConstIterator(ptr + count - 1, this);
#else
		return ConstIterator(ptr + count - 1);
#endif				
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::Iterator Array<T, Allocator>::AheadIterator()
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		return Iterator(ptr - 1, this);
#else
		return Iterator(ptr - 1);
#endif
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::ConstIterator Array<T, Allocator>::AheadIterator() const
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		return ConstIterator(ptr - 1, this);
#else
		return ConstIterator(ptr - 1);
#endif
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::Iterator Array<T, Allocator>::BehindIterator()
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		return Iterator(ptr + count, this);
#else
		return Iterator(ptr + count);
#endif				
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::ConstIterator Array<T, Allocator>::BehindIterator() const
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		return ConstIterator(ptr + count, this);
#else
		return ConstIterator(ptr + count);
#endif				
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::operator ArrayView<std::remove_const_t<T>>() const
	{
		return ArrayView(ptr, count);
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>& Array<T, Allocator>::operator=(const ArrayView<T>& other) requires std::is_copy_assignable_v<T>
	{
		std::destroy_n(ptr, count);

		CopyUnsafe(other.Ptr(), other.Count());

		return *this;
	}	
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>& Array<T, Allocator>::operator=(const Array& other) requires std::is_copy_assignable_v<T>
{
		std::destroy_n(ptr, count);

		CopyUnsafe(other.ptr, other.count);

		return *this;
	}

	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>& Array<T, Allocator>::operator=(Array&& other) noexcept
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
	inline void Array<T, Allocator>::CopyUnsafe(const T* src, uintMem count)
	{
		if (count != 0)
		{
			if (auto newPtr = ReallocateUnsafe(count))
			{
				allocator.Free(ptr);

				ptr = newPtr;
			}

			for (uintMem i = 0; i < count; ++i)
				std::construct_at(ptr + i, src[i]);

			this->count = count;
		}
		else
		{
			this->count = 0;
			this->ptr = nullptr;
		}
	}

	template<typename T, AllocatorType Allocator>
	inline T* Array<T, Allocator>::ReallocateUnsafe(uintMem newCount)
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

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (iteratorCount > 0)
			Debug::Logger::LogWarning("Blaze Engine", "Changing an array while some iterators are referencing it");
#endif

		return (T*)allocator.Allocate(reserved * sizeof(T));
	}

	template<typename T, AllocatorType Allocator>
	Array<T, Allocator>::Iterator begin(Array<T, Allocator>& arr)
	{
		return arr.FirstIterator();
	}
	template<typename T, AllocatorType Allocator>
	Array<T, Allocator>::ConstIterator begin(const Array<T, Allocator>& arr)
	{
		return arr.FirstIterator();
	}
	template<typename T, AllocatorType Allocator>
	Array<T, Allocator>::Iterator end(Array<T, Allocator>& arr)
	{
		return arr.BehindIterator();
	}
	template<typename T, AllocatorType Allocator>
	Array<T, Allocator>::ConstIterator end(const Array<T, Allocator>& arr)
	{
		return arr.BehindIterator();
	}	
}