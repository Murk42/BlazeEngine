#pragma once

namespace Blaze
{	
	template<typename T, AllocatorType Allocator>
	class UnorderedArray;

	template<typename T1, typename T2>
	concept IsConvertibleToUnorderedArrayIterator =
		std::same_as<std::remove_const_t<typename T2::template UnorderedArrayType>, std::remove_const_t<typename T1::template UnorderedArrayType>> &&
		(!(std::is_const_v<typename T1::template UnorderedArrayType> && !std::is_const_v<typename T2::template UnorderedArrayType>));

	template<typename T1, typename T2>
	concept IsComparableToUnorderedArrayIterator =
		std::same_as<std::remove_const_t<typename T2::template UnorderedArrayType>, std::remove_const_t<typename T1::template UnorderedArrayType>>;

	/*
		Used with the Blaze::UnorderedArray class.

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
	template<typename UnorderedArray>
	class BLAZE_CORE_API UnorderedArrayIterator
	{
	public:
		using UnorderedArrayType = UnorderedArray;
		using ValueType = std::conditional_t<std::is_const_v<UnorderedArray>, const typename UnorderedArray::template ValueType, typename UnorderedArray::template ValueType>;
		using value_type = ValueType;

		UnorderedArrayIterator();
		template<IsConvertibleToUnorderedArrayIterator<UnorderedArrayIterator<UnorderedArray>> T>
		UnorderedArrayIterator(const T&);
		~UnorderedArrayIterator();

		bool IsNull() const;

		UnorderedArrayIterator& operator++();
		UnorderedArrayIterator operator++(int);
		UnorderedArrayIterator& operator--();
		UnorderedArrayIterator operator--(int);

		UnorderedArrayIterator operator+(int) const;
		UnorderedArrayIterator operator-(int) const;

		UnorderedArrayIterator& operator+=(int offset);
		UnorderedArrayIterator& operator-=(int offset);

		ValueType& operator*() const;
		ValueType* operator->() const;

		template<IsComparableToUnorderedArrayIterator<UnorderedArrayIterator<UnorderedArray>> T>
		bool operator>(const T& i) const;
		template<IsComparableToUnorderedArrayIterator<UnorderedArrayIterator<UnorderedArray>> T>
		bool operator>=(const T& i) const;
		template<IsComparableToUnorderedArrayIterator<UnorderedArrayIterator<UnorderedArray>> T>
		bool operator<(const T& i) const;
		template<IsComparableToUnorderedArrayIterator<UnorderedArrayIterator<UnorderedArray>> T>
		bool operator<=(const T& i) const;

		template<IsComparableToUnorderedArrayIterator<UnorderedArrayIterator<UnorderedArray>> T>
		bool operator==(const T& i) const;
		template<IsComparableToUnorderedArrayIterator<UnorderedArrayIterator<UnorderedArray>> T>
		bool operator!=(const T& i) const;

		template<IsConvertibleToUnorderedArrayIterator<UnorderedArrayIterator<UnorderedArray>> T>
		UnorderedArrayIterator& operator=(const T& i);

		template<typename, AllocatorType>
		friend class ::Blaze::UnorderedArray;		
	private:
		ValueType* ptr;


#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		UnorderedArray* arr;

		UnorderedArrayIterator(ValueType* ptr, UnorderedArray* arr);
#else
		UnorderedArrayIterator(ValueType* ptr);
#endif
	};

	/*
		Unordered array class

		BLAZE_INVALID_ITERATOR_CHECK:
		If BLAZE_INVALID_ITERATOR_CHECK is defined, passing an invalid iterator or index to a unordered array will result in a fatal error. Having this
		undefined will slightly improve performance. It IS defined in debug mode by default (not defined in release mode).

		BLAZE_CONTAINER_INVALIDATION_CHECK:
		By changing a unordered array that an unordered array iterator is pointing to (adding, erasing, appending, resizing, clearing, etc...) and therefore possibly
		changing the unordered arrays buffer location, the iterator becomes invalid. If BLAZE_CONTAINER_INVALIDATION_CHECK is defined and any operation
		noted above is executed and there is a unordered array iterator referencing the unordered array, a warning will be logged. Otherwise it will try to continue
		execution and have a possible silent fail. When this macro is defined, the size of the unordered array iterator and the unordered array itself change because
		additional information is needed to be stored for this macro to work. Having this undefined will improve performance. It is NOT defined
		in debug mode by default (not defined in release mode either).

		No other macros change the unordered array behaviour
	*/
	template<typename T, AllocatorType Allocator = Blaze::DefaultAllocator>
	class BLAZE_CORE_API UnorderedArray
	{
	public:
		using Iterator = UnorderedArrayIterator<UnorderedArray>;
		using ConstIterator = UnorderedArrayIterator<const UnorderedArray>;
		using ValueType = T;
		using value_type = ValueType;

		UnorderedArray();
		UnorderedArray(const UnorderedArray& arr);
		UnorderedArray(UnorderedArray&& arr) noexcept;

		template<typename ... Args> requires std::constructible_from<T, Args...>
		UnorderedArray(uintMem count, const Args& ... args);
		template<uintMem S>
		UnorderedArray(const T(&arr)[S]);
		UnorderedArray(const T* ptr, uintMem count);
		UnorderedArray(const std::initializer_list<T>& arr);
		UnorderedArray(const ArrayView<std::remove_const_t<T>>& arr);
		UnorderedArray(const ArrayView<const std::remove_const_t<T>>& arr);

		~UnorderedArray();

		void Clear();
		bool Empty() const;
		uintMem Count() const;
		
		template<typename ... Args> requires std::constructible_from<T, Args...>
		Iterator Add(Args&& ... args);
		
		void EraseAt(uintMem index);
		void EraseAt(Iterator it);

		void Append(const UnorderedArray& other);
		void Append(UnorderedArray&& other);

		template<typename ... Args> requires std::constructible_from<T, Args...>
		void Resize(uintMem newCount, Args&& ... args);

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
			Returns an iterator pointing to the first element in the unordered array. If the unordered array is empty returns a null unordered array iterator.
		*/
		Iterator FirstIterator();
		/*
			Returns an iterator pointing to the first element in the unordered array. If the unordered array is empty returns a null unordered array iterator.
		*/
		ConstIterator FirstIterator() const;
		/*
			Returns an iterator pointing to the last element in the unordered array. If the unordered array is empty returns a null unordered array iterator.
		*/
		Iterator LastIterator();
		/*
			Returns an iterator pointing to the last element in the unordered array. If the unordered array is empty returns a null unordered array iterator.
		*/
		ConstIterator LastIterator() const;
		/*
			Returns an iterator pointing one element behind the unordered array. If the unordered array is empty returns a null unordered array iterator.
		*/
		Iterator AheadIterator();
		/*
			Returns an iterator pointing one element ahead the unordered array. If the unordered array is empty returns a null unordered array iterator.
		*/
		ConstIterator AheadIterator() const;
		/*
			Returns an iterator pointing one element behind the unordered array. If the unordered array is empty returns a null unordered array iterator.
		*/
		Iterator BehindIterator();
		/*
			Returns an iterator pointing one element behind the unordered array. If the unordered array is empty returns a null unordered array iterator.
		*/
		ConstIterator BehindIterator() const;

		operator ArrayView<std::remove_const_t<T>>();		

		UnorderedArray& operator=(const ArrayView<std::remove_const_t<T>>& other);
		UnorderedArray& operator=(const ArrayView<const std::remove_const_t<T>>& other);
		UnorderedArray& operator=(const UnorderedArray& other);
		UnorderedArray& operator=(UnorderedArray&& other) noexcept;

		template<typename>
		friend class ArrayIterator;
	private:
		T* ptr;
		uintMem count;
		uintMem reserved;
		BLAZE_ALLOCATOR_ATTRIBUTE Allocator allocator;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		uintMem iteratorCount;
#endif

		/*

			Copies <count> elements starting at <src> without destroying previous
			elements. Changes the current buffer, so the old buffer is freed and a
			new one allocated if needed.

		\param
			src - start of the source unordered array to copy from
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