#pragma once

namespace Blaze
{
	template<typename, AllocatorType>
	class Array;

	template<typename>
	class ArrayView;

	template<typename T1, typename T2>
	concept IsConvertibleToArrayIterator =
		std::same_as<std::remove_const_t<typename T2::template ArrayType>, std::remove_const_t<typename T1::template ArrayType>> &&
		(!(std::is_const_v<typename T1::template ArrayType> && !std::is_const_v<typename T2::template ArrayType>));	

	template<typename T1, typename T2>
	concept IsComparableToArrayIterator =
		std::same_as<std::remove_const_t<typename T2::template ArrayType>, std::remove_const_t<typename T1::template ArrayType>>;

	/*
		Used with the Blaze::Array class.
		
		Iterator usage:
		Increasing, decreasing or dereferencing the iterator outside of valid range or invalidating the arrays previous buffer while some 
		iterators are referencing it cannot be detected if no debugging measures are taken. It is up to the client to ensure its valid use. 		
		
		Null iterators can be created by the default constructor, by copying/assigning an null iterator. They cannot be dereferenced, 
		incremented or decremented, it will result in an error or a possible silent fail, depending if the BLAZE_NULL_ITERATOR_CHECK macro 
		is defined.		

		BLAZE_NULL_ITERATOR_CHECK:
		If BLAZE_NULL_ITERATOR_CHECK is defined, incrementing, decrementing or dereferencing a null array iterator will result in a fatal error.
		Otherwise it will try to continue execution and have a possible silent fail. Having this undefined will slightly improve performance.
		It IS defined in debug mode by default (it isn't defined in release mode).

		BLAZE_CONTAINER_INVALIDATION_CHECK:
		By changing a array that an array iterator is pointing to (adding, erasing, appending, resizing, clearing) and therefore possibly 
		changing the arrays buffer location, the iterator becomes invalid. If BLAZE_CONTAINER_INVALIDATION_CHECK is defined and any operation
		noted above is executed and there is a array iterator referencing the array, a warning will be logged. Otherwise it will try to continue 
		execution and have a possible silent fail. When this macro is defined the size of the array iterator and the array itself changes. 
		Having this undefined will improve performance. It is NOT defined in debug mode by default (not defined in release mode either).

		No other macros change the array iterator behaviour
	*/
	template<typename Array>
	class BLAZE_API ArrayIterator
	{
	public:
		using ArrayType = Array;
		using ValueType = std::conditional_t<std::is_const_v<Array>, const typename Array::template ValueType, typename Array::template ValueType>;
		using value_type = ValueType;

		ArrayIterator();
		template<IsConvertibleToArrayIterator<ArrayIterator<Array>> T>
		ArrayIterator(const T&);
		~ArrayIterator();
		
		bool IsNull() const;				

		ArrayIterator& operator++();
		ArrayIterator operator++(int);
		ArrayIterator& operator--();
		ArrayIterator operator--(int);

		ArrayIterator operator+(int) const;
		ArrayIterator operator-(int) const;

		ArrayIterator& operator+=(int offset);
		ArrayIterator& operator-=(int offset);

		ValueType& operator*() const;
		ValueType* operator->() const;

		template<IsComparableToArrayIterator<ArrayIterator<Array>> T>
		bool operator>(const T& i) const;
		template<IsComparableToArrayIterator<ArrayIterator<Array>> T>
		bool operator>=(const T& i) const;
		template<IsComparableToArrayIterator<ArrayIterator<Array>> T>
		bool operator<(const T& i) const;
		template<IsComparableToArrayIterator<ArrayIterator<Array>> T>
		bool operator<=(const T& i) const;

		template<IsComparableToArrayIterator<ArrayIterator<Array>> T>
		bool operator==(const T& i) const;
		template<IsComparableToArrayIterator<ArrayIterator<Array>> T>
		bool operator!=(const T& i) const;

		template<IsConvertibleToArrayIterator<ArrayIterator<Array>> T>
		ArrayIterator& operator=(const T& i);

		template<typename, AllocatorType>
		friend class ::Blaze::Array;
		template<typename>
		friend class ::Blaze::ArrayView;
	private:		
		ValueType* ptr;


#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		Array* arr;		

		ArrayIterator(ValueType* ptr, Array* arr);
#else
		ArrayIterator(ValueType* ptr);
#endif
	};

	/*
		Basic array class		

		BLAZE_INVALID_ITERATOR_CHECK:
		If BLAZE_INVALID_ITERATOR_CHECK is defined, passing an invalid iterator or index to a array will result in a fatal error. Having this 
		undefined will slightly improve performance. It IS defined in debug mode by default (not defined in release mode).

		BLAZE_CONTAINER_INVALIDATION_CHECK:
		By changing a array that an array iterator is pointing to (adding, erasing, appending, resizing, clearing, etc...) and therefore possibly 
		changing the arrays buffer location, the iterator becomes invalid. If BLAZE_CONTAINER_INVALIDATION_CHECK is defined and any operation
		noted above is executed and there is a array iterator referencing the array, a warning will be logged. Otherwise it will try to continue 
		execution and have a possible silent fail. When this macro is defined, the size of the array iterator and the array itself change because
		additional information is needed to be stored for this macro to work. Having this undefined will improve performance. It is NOT defined 
		in debug mode by default (not defined in release mode either).

		No other macros change the array behaviour
	*/
	template<typename T, AllocatorType Allocator = Blaze::DefaultAllocator>
	class BLAZE_API Array
	{
	public:
		using Iterator = ArrayIterator<Array>;
		using ConstIterator = ArrayIterator<const Array>;
		using ValueType = T;		
		using value_type = ValueType;

		Array();
		Array(const Array& arr) requires std::is_copy_constructible_v<T>;
		Array(Array&& arr) noexcept;

		template<typename ... Args> requires std::constructible_from<T, Args...>
		Array(uintMem count, const Args& ... args);
		template<typename F> requires std::invocable<F, T*, uintMem>
		Array(const F& constructFunction, uintMem count);
		template<uintMem S>
		Array(const T (&arr)[S]) requires std::is_copy_constructible_v<T>;
		Array(const T* ptr, uintMem count) requires std::is_copy_constructible_v<T>;
		Array(const std::initializer_list<T>& arr) requires std::is_copy_constructible_v<T>;
		Array(const ArrayView<std::remove_const_t<T>>& arr) requires std::is_copy_constructible_v<T>;

		~Array();

		void Clear();
		bool Empty() const;
		uintMem Count() const;

		template<typename ... Args> requires std::constructible_from<T, Args...>
		Iterator AddBack(Args&& ... args);				
		template<typename ... Args> requires std::constructible_from<T, Args...>
		Iterator AddAt(uintMem index, Args&& ... args);
		template<typename ... Args> requires std::constructible_from<T, Args...>
		Iterator AddAt(Iterator it, Args&& ... args);
		
		void EraseLast();		
		void EraseAt(uintMem index);
		void EraseAt(Iterator it);
		
		void Append(const Array& other);
		void Append(Array&& other);
		
		template<typename ... Args> requires std::constructible_from<T, Args...>
		void Resize(uintMem newCount, const Args& ... args);
		template<typename F> requires std::invocable<F, T*, uintMem>
		void ResizeWithFunction(uintMem newCount, const F& constructFunction);

		void Truncate(uintMem newCount);
	
		T& operator[](uintMem i);
		const T& operator[](uintMem i) const;
		
		T* Ptr();
		const T* Ptr() const;		

		T& First();
		const T& First() const;
		T& Last();
		const T& Last() const;

		Iterator GetIterator(uintMem index);
		ConstIterator GetIterator(uintMem index) const;

		/*
			Returns an iterator pointing to the first element in the array. If the array is empty returns a null array iterator.
		*/
		Iterator FirstIterator();
		/*
			Returns an iterator pointing to the first element in the array. If the array is empty returns a null array iterator.
		*/
		ConstIterator FirstIterator() const;
		/*
			Returns an iterator pointing to the last element in the array. If the array is empty returns a null array iterator.
		*/
		Iterator LastIterator();
		/*
			Returns an iterator pointing to the last element in the array. If the array is empty returns a null array iterator.
		*/
		ConstIterator LastIterator() const;
		/*
			Returns an iterator pointing one element behind the array. If the array is empty returns a null array iterator.
		*/
		Iterator AheadIterator();
		/*
			Returns an iterator pointing one element ahead the array. If the array is empty returns a null array iterator.
		*/
		ConstIterator AheadIterator() const;
		/*
			Returns an iterator pointing one element behind the array. If the array is empty returns a null array iterator.
		*/
		Iterator BehindIterator();
		/*
			Returns an iterator pointing one element behind the array. If the array is empty returns a null array iterator.
		*/
		ConstIterator BehindIterator() const;
		
		operator ArrayView<std::remove_const_t<T>>() const;		

		Array& operator=(const ArrayView<T>& other) requires std::is_copy_assignable_v<T>;
		Array& operator=(const Array& other) requires std::is_copy_assignable_v<T>;
		Array& operator=(Array&& other) noexcept;		

		template<typename>
		friend class ArrayIterator;
	private:
		T* ptr;
		uintMem count;
		uintMem reserved;
		BLAZE_ALLOCATOR_ATTRIBUTE Allocator allocator;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		uint32 iteratorCount;
#endif

		/*

			Copies <count> elements starting at <src> without destroying previous
			elements. Changes the current buffer, so the old buffer is freed and a 
			new one allocated if needed.

		\param
			src - start of the source array to copy from
		\param
			count - number of elements to copy

		*/
		void CopyUnsafe(const T* src, uintMem count);

		/*
		
			Allocates a new buffer if the old one needs to be changed, returns nullptr 
			otherwise. Only changes the 'reserved' variable. Doesn't move, copy or 
			destroy anything in the old or the new buffer, just allocates the memory.
			Doesn't free the old buffer.
		
		\param
			newCount - number of elements that will be able to fit in the new buffer

		\returns
			A pointer to a new buffer if the old one needs to be changed. Nullptr otherwise.

		*/
		T* ReallocateUnsafe(uintMem newCount);
		
	};

}