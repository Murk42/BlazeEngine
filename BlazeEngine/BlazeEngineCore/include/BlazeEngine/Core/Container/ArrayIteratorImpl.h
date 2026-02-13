#pragma once
#include "BlazeEngine/Core/Debug/Logger.h"

namespace Blaze
{
	template<typename T>
	inline ArrayIterator<T>::ArrayIterator()
		: ptr(nullptr)
	{
	}
	template<typename T>
	inline ArrayIterator<T>::ArrayIterator(const ArrayIterator& other)
		: ptr(other.ptr)
	{
	}
	template<typename T>
	inline ArrayIterator<T>::ArrayIterator(StoredType* ptr)
		: ptr(ptr)
	{
	}
	template<typename T>
	inline ArrayIterator<T>::~ArrayIterator()
	{
	}
	template<typename T>
	inline auto ArrayIterator<T>::Ptr() const -> RemoveReference<T>*
	{
		return &*ptr; //We dereference and then take the address to take into account when T is a reference type
	}
	template<typename T>
	inline bool ArrayIterator<T>::IsNull() const
	{
		return ptr == nullptr;
	}
	template<typename T>
	inline T& ArrayIterator<T>::operator*() const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (IsNull())
			BLAZE_LOG_FATAL("Dereferencing a null iterator");
#endif

		return *ptr;
	}
	template<typename T>
	inline RemoveReference<T>* ArrayIterator<T>::operator->() const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (IsNull())
			BLAZE_LOG_FATAL("Dereferencing a null iterator");
#endif

		return &*ptr; //We dereference and then take the address to take into account when T is a reference type
	}
	template<typename T>
	inline ArrayIterator<T>& ArrayIterator<T>::operator++()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (IsNull())
			BLAZE_LOG_FATAL("Incrementing a null iterator");
#endif

		++ptr;
		return *this;
	}
	template<typename T>
	inline ArrayIterator<T> ArrayIterator<T>::operator++(int)
	{
		ArrayIterator copy = *this;
		++copy;
		return copy;
	}
	template<typename T>
	inline ArrayIterator<T>& ArrayIterator<T>::operator--()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (IsNull())
			BLAZE_LOG_FATAL("Decrementing a null iterator");
#endif

		--ptr;
		return *this;
	}
	template<typename T>
	inline ArrayIterator<T> ArrayIterator<T>::operator--(int)
	{
		ArrayIterator copy = *this;
		--copy;
		return copy;
	}
	template<typename T>
	inline ArrayIterator<T> ArrayIterator<T>::operator+(intMem offset) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (IsNull())
			BLAZE_LOG_FATAL("Adding to a null iterator");
#endif
		return ArrayIterator(ptr + offset);
	}
	template<typename T>
	inline ArrayIterator<T> ArrayIterator<T>::operator-(intMem offset) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (IsNull())
			BLAZE_LOG_FATAL("Subtracting from a null iterator");
#endif
		return ArrayIterator(ptr - offset);
	}
	template<typename T>
	inline uintMem ArrayIterator<T>::operator-(const ArrayIterator<T>& other) const
	{
		return ptr - other.ptr;
	}
	template<typename T>
	inline ArrayIterator<T>& ArrayIterator<T>::operator+=(intMem offset)
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (IsNull())
			BLAZE_LOG_FATAL("Adding to a null iterator");
#endif
		ptr += offset;

		return *this;
	}
	template<typename T>
	inline ArrayIterator<T>& ArrayIterator<T>::operator-=(intMem offset)
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (IsNull())
			BLAZE_LOG_FATAL("Subtracting from a null iterator");
#endif
		ptr -= offset;

		return *this;
	}
	template<typename T>
	inline bool ArrayIterator<T>::operator>(const ArrayIterator& other) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (IsNull() != other.IsNull())
			BLAZE_LOG_FATAL("Comparing a null iterator");
#endif

		return ptr > other.ptr;
	}
	template<typename T>
	inline bool ArrayIterator<T>::operator>=(const ArrayIterator& other) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (IsNull() != other.IsNull())
			BLAZE_LOG_FATAL("Comparing a null iterator");
#endif

		return ptr >= other.ptr;
	}
	template<typename T>
	inline bool ArrayIterator<T>::operator<(const ArrayIterator& other) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (IsNull() != other.IsNull())
			BLAZE_LOG_FATAL("Comparing a null iterator");
#endif

		return ptr < other.ptr;
	}
	template<typename T>
	inline bool ArrayIterator<T>::operator<=(const ArrayIterator& other) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (IsNull() != other.IsNull())
			BLAZE_LOG_FATAL("Comparing a null iterator");
#endif

		return ptr <= other.ptr;
	}
	template<typename T>
	inline bool ArrayIterator<T>::operator==(const ArrayIterator& other) const
	{
		return ptr == other.ptr;
	}
	template<typename T>
	inline bool ArrayIterator<T>::operator!=(const ArrayIterator& other) const
	{
		return ptr != other.ptr;
	}
	template<typename T>
	inline Blaze::ArrayIterator<T>::operator ArrayIterator<const T>() const
	{
		return ArrayIterator<const T>(ptr);
	}
	template<typename T>
	inline Blaze::ArrayIterator<T>::operator const RemoveReference<T>* () const
	{
		return &*ptr;//We dereference and then take the address to take into account when T is a reference type;
	}
	template<typename T>
	inline Blaze::ArrayIterator<T>::operator RemoveReference<T>* () const requires (!IsConstType<T>)
	{
		return &*ptr; //We dereference and then take the address to take into account when T is a reference type
	}
	template<typename T>
	inline ArrayIterator<T>& ArrayIterator<T>::operator=(const ArrayIterator& other)
	{
		ptr = other.ptr;

		return *this;
	}
}