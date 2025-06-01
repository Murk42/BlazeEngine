#pragma once
#include "BlazeEngineCore/DataStructures/ArrayView.h"
#include "BlazeEngineCore/DataStructures/ArrayIterator.h"
#include "BlazeEngineCore/Debug/Logger.h"

namespace Blaze
{
	template<typename T>
	inline constexpr ArrayView<T>::ArrayView()
		: ptr(nullptr), count(0)
	{
	}
	template<typename T>
	inline constexpr ArrayView<T>::ArrayView(const ArrayView& other)
		: ptr(other.ptr), count(other.count)
	{
	}
	template<typename T>
	inline constexpr ArrayView<T>::ArrayView(Iterator begin, Iterator end)
		: ptr(nullptr), count(0)
	{
		if (begin.IsNull() || end.IsNull())
		{
			ptr = nullptr;
			count = 0;
		}
		else
		{
			ptr = begin.Ptr();
			count = end.Ptr() - begin.Ptr();
		}
	}
	template<typename T>
	template<uintMem S>
	inline constexpr ArrayView<T>::ArrayView(const StoredType(&arr)[S])
		: ptr(arr), count(S)
	{
	}
	template<typename T>
	inline constexpr ArrayView<T>::ArrayView(const std::initializer_list<StoredType>& list)
		: ptr(nullptr), count(0)
	{
		if (list.begin() == nullptr || list.size() == 0)
			return;

		this->ptr = list.begin();
		this->count = list.size();
	}
	template<typename T>
	inline constexpr ArrayView<T>::ArrayView(const StoredType* ptr, uintMem count)
		: ptr(nullptr), count(0)
	{
		if (ptr == nullptr || count == 0)
			return;

		this->ptr = ptr;
		this->count = count;
	}			
	template<typename T>
	inline constexpr void ArrayView<T>::Clear()
	{
		ptr = nullptr;
		count = 0;
	}
	template<typename T>
	inline constexpr bool ArrayView<T>::Empty() const
	{
		return count == 0;
	}
	template<typename T>
	inline constexpr uintMem ArrayView<T>::Count() const
	{
		return count;
	}
	template<typename T>
	inline constexpr const T& ArrayView<T>::operator[](uintMem index) const
	{
		if (index >= count)
			throw;

		if constexpr (std::is_reference_v<T>)
			return ptr[index][0];
		else
			return ptr[index];
	}
	template<typename T>
	inline constexpr const typename ArrayView<T>::template StoredType* ArrayView<T>::Ptr() const
	{
		return ptr;
	}	
	template<typename T>
	inline const T& ArrayView<T>::First() const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			BLAZE_ENGINE_CORE_FATAL("Array is empty");
#endif

		if constexpr (std::is_reference_v<T>)
			return ptr[0][0];
		else
			return ptr[0];
	}
	template<typename T>
	inline const T& ArrayView<T>::Last() const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			BLAZE_ENGINE_CORE_FATAL("Array is empty");
#endif

		if constexpr (std::is_reference_v<T>)
			return ptr[count - 1][0];
		else
			return ptr[count - 1];
	}
	template<typename T>
	inline ArrayView<T>::Iterator ArrayView<T>::GetIterator(uintMem index) const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (index >= count)
			BLAZE_ENGINE_CORE_FATAL("Index out of range");
#endif		
		return Iterator(ptr + index);
	}
	template<typename T>
	inline ArrayView<T>::Iterator ArrayView<T>::FirstIterator() const
	{
		return Iterator(ptr);
	}
	template<typename T>
	inline ArrayView<T>::Iterator ArrayView<T>::LastIterator() const
	{
		return Iterator(ptr + count - 1);
	}

	template<typename T>
	inline ArrayView<T>::Iterator ArrayView<T>::AheadIterator() const
	{		
		return Iterator(ptr - 1);
	}

	template<typename T>
	inline ArrayView<T>::Iterator ArrayView<T>::BehindIterator() const
	{
		return Iterator(ptr + count);
	}

	template<typename T>
	inline constexpr bool ArrayView<T>::operator==(const ArrayView<T>& other) const requires std::equality_comparable<T> || std::is_reference_v<T>
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

	template<typename T>
	inline constexpr bool ArrayView<T>::operator!=(const ArrayView<T>& other) const requires std::equality_comparable<T> || std::is_reference_v<T>
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

	template<typename T>
	inline constexpr ArrayView<T>& ArrayView<T>::operator=(const ArrayView& other)
	{
		ptr = other.ptr;
		count = other.count;
		return *this;
	}
	template<typename T>
	template<AllocatorType Allocator>
	inline constexpr ArrayView<T>& Blaze::ArrayView<T>::operator=(const Array<T, Allocator>& array)
	{
		ptr = array.Ptr();
		count = array.Count();

		return *this;
	}	
}