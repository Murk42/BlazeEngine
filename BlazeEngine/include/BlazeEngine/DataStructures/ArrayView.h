#pragma once

namespace Blaze
{	
	template<typename T, AllocatorType Allocator>
	class Array;

	template<typename T>
	class ArrayView
	{		
	public:	
		static_assert(!std::is_const_v<T>, "T shouldn't be const because const achieves nothing and makes using ArrayView much harder");

		using Iterator = ArrayIterator<const ArrayView>;
		using ConstIterator = ArrayIterator<const ArrayView>;
		using ValueType = T;
		using value_type = ValueType;

		constexpr ArrayView();
		constexpr ArrayView(const ArrayView&);
		constexpr ArrayView(const T* ptr, uintMem count);
		template<uintMem S>
		constexpr ArrayView(const T (&arr)[S]);		

		constexpr void Clear();
		constexpr bool Empty() const;
		constexpr uintMem Count() const;

		constexpr const T& operator[](uintMem index) const;

		constexpr const T* Ptr() const;				
		
		const T& First() const;		
		const T& Last() const;
		
		Iterator GetIterator(uintMem index) const;

		/*
			Returns an iterator pointing to the first element in the array. If the array is empty returns a null array iterator.
		*/
		Iterator FirstIterator() const;	
		/*
			Returns an iterator pointing to the last element in the array. If the array is empty returns a null array iterator.
		*/
		Iterator LastIterator() const;
		/*
			Returns an iterator pointing to the last element in the array. If the array is empty returns a null array iterator.
		*/		
		Iterator AheadIterator() const;		
		/*
			Returns an iterator pointing one element behind the array. If the array is empty returns a null array iterator.
		*/
		Iterator BehindIterator() const;		

		constexpr ArrayView& operator=(const ArrayView& array);
		template<AllocatorType Allocator>
		constexpr ArrayView& operator=(const Array<T, Allocator>& array);		

		template<typename>
		friend class ArrayIterator;
	private:
		const T* ptr;
		uintMem count;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		uint32 iteratorCount;
#endif
	};		

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
	inline constexpr ArrayView<T>::ArrayView(const T* ptr, uintMem count)
		: ptr(ptr), count(count)
	{
	}
	template<typename T>
	template<uintMem S>
	inline constexpr ArrayView<T>::ArrayView(const T(&arr)[S])
		: ptr(arr), count(S)
	{
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
		return ptr[index];
	}
	template<typename T>
	inline constexpr const T* ArrayView<T>::Ptr() const 
	{
		return ptr; 
	}
	template<typename T>
	inline const T& ArrayView<T>::First() const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			Debug::Logger::LogFatal("Blaze Engine", "Array is empty");
#endif

		return ptr[0];
	}
	template<typename T>
	inline const T& ArrayView<T>::Last() const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			Debug::Logger::LogFatal("Blaze Engine", "Array is empty");
#endif

		return ptr[count - 1];
	}
	template<typename T>
	inline ArrayView<T>::Iterator ArrayView<T>::GetIterator(uintMem index) const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (index >= count)
			Debug::Logger::LogFatal("Blaze Engine", "Index out of range");
#endif		
		return Iterator(ptr + index);
	}
	template<typename T>
	inline ArrayView<T>::Iterator ArrayView<T>::FirstIterator() const
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		return Iterator(ptr, this);
#else
		return Iterator(ptr);
#endif			
	}
	template<typename T>
	inline ArrayView<T>::Iterator ArrayView<T>::LastIterator() const
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		return Iterator(ptr + count - 1, this);
#else
		return Iterator(ptr + count - 1);
#endif			
	}

	template<typename T>
	inline ArrayView<T>::Iterator ArrayView<T>::AheadIterator() const
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		return Iterator(ptr - 1, this);
#else
		return Iterator(ptr - 1);
#endif			
	}

	template<typename T>
	inline ArrayView<T>::Iterator ArrayView<T>::BehindIterator() const
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		return Iterator(ptr + count, this);
#else
		return Iterator(ptr + count);
#endif			
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
	template<typename T>
	inline constexpr ArrayView<T>::Iterator begin(const ArrayView<T>& arr) 
	{
		return arr.FirstIterator();
	}
	template<typename T>
	inline constexpr ArrayView<T>::Iterator end(const ArrayView<T>& arr)
	{
		return arr.BehindIterator();
	}
}