#pragma once
#include "BlazeEngineCore/DataStructures/ArrayIterator.h"
#include "BlazeEngineCore/Debug/Logger.h"

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
	inline typename ArrayIterator<T>::template StoredType* ArrayIterator<T>::Ptr() const
	{
		return ptr;
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
		if (ptr == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Dereferencing a null iterator");
#endif	

		if constexpr (std::is_reference_v<T>)
			return **ptr;
		else
			return *ptr;
	}	
	template<typename T>
	inline typename ArrayIterator<T>::template StoredType* ArrayIterator<T>::operator->() const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Dereferencing a null iterator");
#endif
		
		return ptr;
	}	
	template<typename T>
	inline ArrayIterator<T>& ArrayIterator<T>::operator++()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Incrementing a null iterator");
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
		if (ptr == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Decrementing a null iterator");
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
	inline ArrayIterator<T> ArrayIterator<T>::operator+(int offset) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Adding to a null iterator");
#endif		
		return ArrayIterator(ptr + offset);
	}
	template<typename T>
	inline ArrayIterator<T> ArrayIterator<T>::operator-(int offset) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Subtracting from a null iterator");
#endif		
		return ArrayIterator(ptr - offset);
	}
	template<typename T>
	inline uintMem ArrayIterator<T>::operator-(const ArrayIterator<T>& other) const
	{
		return ptr - other.ptr;
	}
	template<typename T>
	inline ArrayIterator<T>& ArrayIterator<T>::operator+=(int offset)
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Adding to a null iterator");
#endif		
		ptr += offset;

		return *this;
	}
	template<typename T>
	inline ArrayIterator<T>& ArrayIterator<T>::operator-=(int offset)
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (ptr == nullptr)
			BLAZE_ENGINE_CORE_FATAL("Subtracting from a null iterator");
#endif		
		ptr -= offset;

		return *this;
	}
	template<typename T>	
	inline bool ArrayIterator<T>::operator>(const ArrayIterator& other) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if ((ptr == nullptr) != (other.ptr == nullptr))
			BLAZE_ENGINE_CORE_FATAL("Comparing a null iterator");
#endif

		return ptr > other.ptr;
	}
	template<typename T>	
	inline bool ArrayIterator<T>::operator>=(const ArrayIterator& other) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if ((ptr == nullptr) != (other.ptr == nullptr))
			BLAZE_ENGINE_CORE_FATAL("Comparing a null iterator");
#endif

		return ptr >= other.ptr;
	}
	template<typename T>	
	inline bool ArrayIterator<T>::operator<(const ArrayIterator& other) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if ((ptr == nullptr) != (other.ptr == nullptr))
			BLAZE_ENGINE_CORE_FATAL("Comparing a null iterator");
#endif

		return ptr < other.ptr;
	}
	template<typename T>	
	inline bool ArrayIterator<T>::operator<=(const ArrayIterator& other) const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if ((ptr == nullptr) != (other.ptr == nullptr))
			BLAZE_ENGINE_CORE_FATAL("Comparing a null iterator");
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
	inline ArrayIterator<T>& ArrayIterator<T>::operator=(const ArrayIterator& other)
	{
		ptr = other.ptr;

		return *this;
	}	
}