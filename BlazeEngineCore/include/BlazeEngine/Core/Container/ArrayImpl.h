#pragma once
#include "BlazeEngine/Core/Container/ArrayIterator.h"
#include "BlazeEngine/Core/Debug/Logger.h"
#include <memory>

namespace Blaze
{
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::Array()
		: ptr(nullptr), count(0), reserved(0)
	{
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::Array(const Array& other) requires std::copy_constructible<typename Array<T, Allocator>::StoredType>
		: ptr(nullptr), count(0), reserved(0)
	{
		CopyConstructUnsafe(other.ptr, other.count);
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::Array(Array&& other) noexcept
		: ptr(other.ptr), count(other.count), reserved(other.reserved)
	{
		other.ptr = nullptr;
		other.count = 0;
		other.reserved = 0;
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::Array(const ArrayView<T>& other) requires std::copy_constructible<typename Array<T, Allocator>::StoredType>
		: ptr(nullptr), count(0), reserved(0)
	{
		CopyConstructUnsafe(other.Ptr(), other.Count());
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::Array(const StoredType* begin, const StoredType* end) requires std::copy_constructible<typename Array<T, Allocator>::StoredType>
		: ptr(nullptr), count(0), reserved(0)
	{
		if (begin == nullptr || end == nullptr)
			return;

		CopyConstructUnsafe(begin, end - begin);
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::Array(Iterator begin, Iterator end) requires std::copy_constructible<typename Array<T, Allocator>::StoredType>
		: ptr(nullptr), count(0), reserved(0)
	{
		if (begin.IsNull() || end.IsNull())
			return;

		CopyConstructUnsafe(begin.Ptr(), end.Ptr() - begin.Ptr());
	}
	template<typename T, AllocatorType Allocator>
	template<typename F> requires std::invocable<F, typename Array<T, Allocator>::StoredType*, uintMem>
	inline Array<T, Allocator>::Array(uintMem count, const F& constructFunction)
		: Array()
	{
		ResizeWithFunction(count, constructFunction);
	}
	template<typename T, AllocatorType Allocator>
	template<typename ... Args> requires IsConstructibleFrom<typename Array<T, Allocator>::StoredType, Args...>
	inline Array<T, Allocator>::Array(uintMem count, Args&& ... args)
		: Array()
	{
		Resize(count, std::forward<Args>(args)...);
	}
	template<typename T, AllocatorType Allocator>
	template<typename T2, uintMem S> requires IsConvertibleTo<const T2&, typename Array<T, Allocator>::StoredType>
	inline Array<T, Allocator>::Array(const T2(&other)[S])
		: ptr(nullptr), count(0), reserved(0)
	{
		CopyConstructUnsafe(other, S);
	}
	template<typename T, AllocatorType Allocator>
	template<typename T2> requires IsConvertibleTo<const T2&, typename Array<T, Allocator>::StoredType>
	inline Array<T, Allocator>::Array(const std::initializer_list<T2>& other)
		: ptr(nullptr), count(0), reserved(0)
	{
		CopyConstructUnsafe(other.begin(), other.size());
	}
	template<typename T, AllocatorType Allocator>
	template<typename T2> requires IsConvertibleTo<const T2&, typename Array<T, Allocator>::StoredType>
	inline Array<T, Allocator>::Array(const T2* ptr, uintMem count)
		: ptr(nullptr), count(0), reserved(0)
	{
		if (ptr != nullptr)
			if (count != 0)
				CopyConstructUnsafe(ptr, count);
			else
				BLAZE_LOG_FATAL("Creating array with a non nullptr but a count 0");
		else if (count != 0)
		{
			this->ptr = ReallocateUnsafe(count);

			for (uintMem i = 0; i < count; ++i)
				std::construct_at(this->ptr + i);

			this->count = count;
		}
	}
	template<typename T, AllocatorType Allocator>
	template<typename T2, AllocatorType Allocator2> requires IsConvertibleTo<const T2&, typename Array<T, Allocator>::StoredType>
	inline Array<T, Allocator>::Array(const Array<T2, Allocator2>& other)
		: ptr(nullptr), count(0), reserved(0)
	{
		CopyConstructUnsafe(other.ptr, other.count);
	}
	template<typename T, AllocatorType Allocator>
	template<typename T2> requires IsConvertibleTo<const T2&, typename Array<T, Allocator>::StoredType>
	inline Array<T, Allocator>::Array(const ArrayView<T2>& other)
		: ptr(nullptr), count(0), reserved(0)
	{
		CopyConstructUnsafe(other.Ptr(), other.Count());
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::~Array()
	{
		Clear();
	}
	template<typename T, AllocatorType Allocator>
	inline void Array<T, Allocator>::Clear()
	{
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
	template<typename ...Args> requires IsConstructibleFrom<T, Args...>
	inline auto Array<T, Allocator>::AddBack(Args&& ...args) -> Iterator requires std::move_constructible<StoredType>
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

		return LastIterator();
	}
	template<typename T, AllocatorType Allocator>
	template<typename ...Args> requires IsConstructibleFrom<T, Args...>
	inline auto Array<T, Allocator>::TryAddBack(Args && ...args) -> Iterator
	{
		if (count < reserved)
		{
			std::construct_at(ptr + count, std::forward<Args>(args)...);
			++count;

			return Iterator(ptr + count - 1);
		}
		else
			return Iterator();

	}
	template<typename T, AllocatorType Allocator>
	template<typename ...Args> requires IsConstructibleFrom<T, Args...>
	inline auto Array<T, Allocator>::AddAt(uintMem index, Args && ...args) -> Iterator requires std::move_constructible<StoredType>
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (index > count)
			BLAZE_LOG_FATAL("Trying to add an element outside the array");
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

		return Iterator(ptr + index);
	}
	template<typename T, AllocatorType Allocator>
	template<typename ...Args> requires IsConstructibleFrom<T, Args...>
	inline auto Array<T, Allocator>::AddAt(Iterator it, Args && ...args) -> Iterator requires std::move_constructible<StoredType>
	{
		return AddAt(it.Ptr() - ptr, std::forward<Args>(args)...);
	}
	template<typename T, AllocatorType Allocator>
	inline void Array<T, Allocator>::Insert(Iterator it, ArrayView<T> array)  requires std::move_constructible<StoredType>
	{
		Insert(it.Ptr() - ptr, array);
	}
	template<typename T, AllocatorType Allocator>
	inline void Array<T, Allocator>::Insert(uintMem index, ArrayView<T> array)  requires std::move_constructible<StoredType>
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (index > count)
			BLAZE_LOG_FATAL("Trying to add an element outside the array");
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
			uintMem moveEndIndex = array.Count() + index < count ? array.Count() + index : count;

			for (uintMem i = count + moveEndIndex; i > index; --i)
				ptr[i + array.Count() - 1] = std::move(ptr[i - 1]);

			for (uintMem i = 0; i < array.Count(); ++i)
				std::construct_at(ptr + i, array[i]);
		}

		count += array.Count();
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator> Array<T, Allocator>::Split(Iterator it)
	{
		return Split(it.Ptr() - ptr);
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator> Array<T, Allocator>::Split(uintMem index)
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (index > count)
		{
			BLAZE_LOG_FATAL("Trying to add an element outside the array");
			return;
		}
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
	inline void Array<T, Allocator>::EraseLast() requires std::is_move_constructible_v<StoredType>
	{
		if (count == 0)
		{
			BLAZE_LOG_FATAL("Trying to erase the last element of an empty array.");
			return;
		}

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
	inline void Array<T, Allocator>::EraseAt(uintMem index) requires std::is_move_assignable_v<StoredType> || std::is_move_constructible_v<StoredType>
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (index >= count)
			BLAZE_LOG_FATAL("Trying to erase an element outside the array");
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
			if constexpr (std::is_move_assignable_v<StoredType>)
				for (uintMem i = index; i < count - 1; ++i)
					ptr[i] = std::move(ptr[i + 1]);
			else
				for (uintMem i = index; i < count - 1; ++i)
				{
					std::destroy_at(ptr + i);
					std::construct_at(ptr + i, ptr[i + 1]);
				}

			std::destroy_at(ptr + count - 1);
		}

		count--;
	}
	template<typename T, AllocatorType Allocator>
	inline void Array<T, Allocator>::EraseAt(Iterator it) requires std::is_move_assignable_v<StoredType> || std::is_move_constructible_v<StoredType>
	{
		EraseAt(it.Ptr() - ptr);
	}
	template<typename T, AllocatorType Allocator>
	inline void Array<T, Allocator>::Append(const Array& other) requires std::copy_constructible<StoredType>
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
	inline void Array<T, Allocator>::Append(Array&& other) requires std::move_constructible<StoredType>
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
	template<typename ...Args> requires IsConstructibleFrom<T, Args...>
	inline void Array<T, Allocator>::Resize(uintMem newCount, Args&& ...args) requires std::move_constructible<StoredType>
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
	template<typename F> requires std::invocable<F, typename Array<T, Allocator>::StoredType*, uintMem>
	void Array<T, Allocator>::ResizeWithFunction(uintMem newCount, const F& constructFunction) requires std::move_constructible<StoredType>
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
	void Array<T, Allocator>::ReserveAdditional(uintMem reservedCount) requires IsConstructibleFrom<T, T&&>
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
	inline T& Array<T, Allocator>::operator[](uintMem i)
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (i >= count)
			BLAZE_LOG_FATAL("Invalid index");
#endif

		return ptr[i];
	}
	template<typename T, AllocatorType Allocator>
	inline const T& Array<T, Allocator>::operator[](uintMem i) const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (i >= count)
			BLAZE_LOG_FATAL("Invalid index");
#endif
		return ptr[i];
	}
	template<typename T, AllocatorType Allocator>
	inline auto Array<T, Allocator>::Ptr() -> StoredType*
	{
		return ptr;
	}
	template<typename T, AllocatorType Allocator>
	inline auto Array<T, Allocator>::Ptr() const -> const StoredType*
	{
		return ptr;
	}
	template<typename T, AllocatorType Allocator>
	inline auto Array<T, Allocator>::Begin() -> StoredType*
	{
		return ptr;
	}
	template<typename T, AllocatorType Allocator>
	inline auto Array<T, Allocator>::Begin() const -> const StoredType*
	{
		return ptr;
	}
	template<typename T, AllocatorType Allocator>
	inline auto Array<T, Allocator>::End() -> StoredType*
	{
		return ptr + count;
	}
	template<typename T, AllocatorType Allocator>
	inline auto Array<T, Allocator>::End() const -> const StoredType*
	{
		return ptr + count;
	}
	template<typename T, AllocatorType Allocator>
	inline T& Array<T, Allocator>::First()
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			BLAZE_LOG_FATAL("Array is empty");
#endif

		return ptr[0];
	}
	template<typename T, AllocatorType Allocator>
	inline const T& Array<T, Allocator>::First() const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			BLAZE_LOG_FATAL("Array is empty");
#endif

		return ptr[0];
	}
	template<typename T, AllocatorType Allocator>
	inline T& Array<T, Allocator>::Last()
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			BLAZE_LOG_FATAL("Array is empty");
#endif

		return ptr[count - 1];
	}
	template<typename T, AllocatorType Allocator>
	inline const T& Array<T, Allocator>::Last() const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			BLAZE_LOG_FATAL("Array is empty");
#endif

		return ptr[count - 1];
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::Iterator Array<T, Allocator>::GetIterator(uintMem index)
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (index >= count)
			BLAZE_LOG_FATAL("Index out of range");
#endif
		return Iterator(ptr + index);
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::ConstIterator Array<T, Allocator>::GetIterator(uintMem index) const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (index >= count)
			BLAZE_LOG_FATAL("Index out of range");
#endif
		return ConstIterator(ptr + index);
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::Iterator Array<T, Allocator>::FirstIterator()
	{
		return Iterator(ptr);
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::ConstIterator Array<T, Allocator>::FirstIterator() const
	{
		return ConstIterator(ptr);
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::Iterator Array<T, Allocator>::LastIterator()
	{
		return Iterator(ptr + count - 1);
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::ConstIterator Array<T, Allocator>::LastIterator() const
	{
		return ConstIterator(ptr + count - 1);
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::Iterator Array<T, Allocator>::AheadIterator()
	{
		return Iterator(ptr - 1);
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::ConstIterator Array<T, Allocator>::AheadIterator() const
	{
		return ConstIterator(ptr - 1);
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::Iterator Array<T, Allocator>::BehindIterator()
	{
		return Iterator(ptr + count);
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::ConstIterator Array<T, Allocator>::BehindIterator() const
	{
		return ConstIterator(ptr + count);
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>::operator ArrayView<T>() const
	{
		return ArrayView<T>(ptr, count);
	}
	template<typename T, AllocatorType Allocator>
	inline bool Array<T, Allocator>::operator==(const ArrayView<T>& other) const requires std::equality_comparable<typename Array<T, Allocator>::StoredType>
	{
		if (count != other.Count())
			return false;

		auto const end = ptr + count;
		auto it1 = ptr;
		auto it2 = other.Ptr();

		while (it1 != end)
		{
			if (!(*it1 == *it2))
				return false;

			++it1;
			++it2;
		}

		return true;
	}
	template<typename T, AllocatorType Allocator>
	inline bool Array<T, Allocator>::operator!=(const ArrayView<T>& other) const requires std::equality_comparable<typename Array<T, Allocator>::StoredType>
	{
		if (count != other.Count())
			return true;

		auto const end = ptr + count;
		auto it1 = ptr;
		auto it2 = other.Ptr();

		while (it1 != end)
		{
			if (!(*it1 == *it2))
				return true;

			++it1;
			++it2;
		}

		return false;
	}
	template<typename T, AllocatorType Allocator>
	inline Array<T, Allocator>& Array<T, Allocator>::operator=(const Array& other) requires std::copy_constructible<typename Array<T, Allocator>::StoredType>&& std::assignable_from<typename Array<T, Allocator>::StoredType&, const typename Array<T, Allocator>::StoredType&>
	{
		CopyAssign(other.ptr, other.count);

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
	template<typename T2, AllocatorType Allocator2> requires IsConvertibleTo<const T2&, typename Array<T, Allocator>::StoredType> && std::assignable_from<typename Array<T, Allocator>::StoredType&, const T2&>
	inline Array<T, Allocator>& Array<T, Allocator>::operator=(const Array<T2, Allocator2>& other)
	{
		CopyAssign(other.ptr, other.count);

		return *this;
	}
	template<typename T, AllocatorType Allocator>
	template<typename T2> requires IsConvertibleTo<const T2&, typename Array<T, Allocator>::StoredType>&& std::assignable_from<typename Array<T, Allocator>::StoredType&, const T2&>
	inline Array<T, Allocator>& Array<T, Allocator>::operator=(const ArrayView<T2>& other)
	{
		CopyAssign(other.Ptr(), other.Count());

		return *this;
	}
	template<typename T, AllocatorType Allocator>
	template<typename T2> requires IsConvertibleTo<const T2&, typename Array<T, Allocator>::StoredType>
	inline void Array<T, Allocator>::CopyConstructUnsafe(const T2* src, uintMem count)
	{
		if (count != 0)
		{
			if (auto newPtr = ReallocateUnsafe(count))
			{
				allocator.Free(ptr);

				ptr = newPtr;
			}

			for (uintMem i = 0; i < count; ++i)
				std::construct_at(ptr + i, static_cast<T>(src[i]));

			this->count = count;
		}
		else
		{
			allocator.Free(ptr);
			this->count = 0;
			this->ptr = nullptr;
		}
	}
	template<typename T, AllocatorType Allocator>
	template<typename T2> requires IsConvertibleTo<const T2&, typename Array<T, Allocator>::StoredType> && std::assignable_from<typename Array<T, Allocator>::StoredType&, const T2&>
	inline void Array<T, Allocator>::CopyAssign(const T2* src, uintMem newCount)
	{
		if (newCount != 0)
		{
			if (auto newPtr = ReallocateUnsafe(newCount))
			{
				std::destroy_n(ptr, count);
				allocator.Free(ptr);

				ptr = newPtr;

				for (uintMem i = 0; i < newCount; ++i)
					if constexpr (IsConstructibleFrom<T, T2>)
						std::construct_at(ptr + i, src[i]);
					else
						std::construct_at(ptr + i, static_cast<T>(src[i]));
			}
			else if (count > newCount)
			{
				for (uintMem i = 0; i < newCount; ++i)
					if constexpr (std::assignable_from<T, T2>)
						ptr[i] = src[i];
					else
						ptr[i] = static_cast<T>(src[i]);

				std::destroy_n(ptr + newCount, count - newCount);
			}
			else
			{
				for (uintMem i = 0; i < count; ++i)
					if constexpr (std::assignable_from<T, T2>)
						ptr[i] = src[i];
					else
						ptr[i] = (T)src[i];

				for (uintMem i = count; i < newCount; ++i)
					if constexpr (IsConstructibleFrom<T, T2>)
						std::construct_at(ptr + i, src[i]);
					else
						std::construct_at(ptr + i, static_cast<T>(src[i]));
			}

			this->count = newCount;
		}
		else
		{
			std::destroy_n(ptr, count);
			allocator.Free(ptr);
			count = 0;
			ptr = nullptr;
		}
	}
	template<typename T, AllocatorType Allocator>
	inline auto Array<T, Allocator>::ReallocateUnsafe(uintMem newCount) -> StoredType*
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

		return (StoredType*)allocator.Allocate(reserved * sizeof(StoredType));
	}
}