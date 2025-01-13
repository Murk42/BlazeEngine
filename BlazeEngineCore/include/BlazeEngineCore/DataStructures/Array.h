#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"
#include "BlazeEngineCore/Debug/Logger.h"

namespace Blaze
{
	template<typename, AllocatorType>
	class Array;

	template<typename>
	class ArrayView;

	template<typename Left, typename T2>
	concept IsConvertibleToArrayIterator =
		std::same_as<std::remove_const_t<typename T2::template ArrayType>, std::remove_const_t<typename Left::template ArrayType>> &&
		(!(std::is_const_v<typename Left::template ArrayType> && !std::is_const_v<typename T2::template ArrayType>));	

	template<typename Left, typename T2>
	concept IsComparableToArrayIterator =
		std::same_as<std::remove_const_t<typename T2::template ArrayType>, std::remove_const_t<typename Left::template ArrayType>>;

	template<typename Left, typename Right>
	concept IsArrayConstructibleFrom = std::constructible_from<Left, Right> || std::convertible_to<Right, Left>&& std::copy_constructible<Left>;
	template<typename Left, typename Right>
	concept IsArrayAssignableFrom = IsArrayConstructibleFrom<Left, Right> && (std::assignable_from<Left, Right> || std::convertible_to<Right, Left> && std::copyable<Left>);

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
	class BLAZE_CORE_API ArrayIterator
	{
	public:
		using ArrayType = Array;
		using ValueType = std::conditional_t<std::is_const_v<Array>, const typename Array::template ValueType, typename Array::template ValueType>;
		using value_type = ValueType;		
		using InterfaceValueType = std::remove_reference_t<ValueType>;
		using InteranlValueType = std::conditional_t<std::is_const_v<Array>, const typename Array::template InternalValueType, typename Array::template InternalValueType>;

		ArrayIterator();
		template<IsConvertibleToArrayIterator<ArrayIterator<Array>> T>
		ArrayIterator(const T&);
		~ArrayIterator();
		
		bool IsNull() const;				

		std::remove_reference_t<ValueType>& operator*() const;
		std::remove_reference_t<ValueType>* operator->() const;

		ArrayIterator& operator++();
		ArrayIterator operator++(int);
		ArrayIterator& operator--();
		ArrayIterator operator--(int);

		ArrayIterator operator+(int) const;
		ArrayIterator operator-(int) const;

		uintMem operator-(const ArrayIterator& other) const;

		ArrayIterator& operator+=(int offset);
		ArrayIterator& operator-=(int offset);

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
		InteranlValueType* ptr;


#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		Array* arr;		

		ArrayIterator(InteranlValueType* ptr, Array* arr);
#else
		ArrayIterator(InteranlValueType* ptr);
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
	class BLAZE_CORE_API Array
	{
		struct ReferenceHolder
		{
			ReferenceHolder(T& value) : value(value) { }
			ReferenceHolder(const ReferenceHolder& other) : value(other.value) { }
			ReferenceHolder(ReferenceHolder&& other) : value(other.value) { }

			ReferenceHolder& operator=(ReferenceHolder&&) = delete;
			ReferenceHolder& operator=(const ReferenceHolder&) = delete;

			T& value;
		};
	public:
		using Iterator = ArrayIterator<Array>;
		using ConstIterator = ArrayIterator<const Array>;
		using ValueType = T;		
		using value_type = ValueType;		
		using InternalValueType = std::conditional_t<std::is_reference_v<T>, ReferenceHolder, T>;

		Array();
		Array(const Array& other) requires std::copy_constructible<InternalValueType>;
		Array(Array&& other) noexcept;
				
		template<typename T2, AllocatorType Allocator2> 
		Array(const Array<T2, Allocator2>& other) requires IsArrayConstructibleFrom<InternalValueType, const typename Array<T2, Allocator2>::template InternalValueType&>;
		template<typename ... Args>
		Array(uintMem count, Args&& ... args) requires std::constructible_from<InternalValueType, Args...>;
		template<typename F>
		Array(uintMem count, const F& constructFunction) requires std::invocable<F, InternalValueType*, uintMem>;
		template<typename T2, uintMem S> 
		Array(const T2(&arr)[S]) requires IsArrayConstructibleFrom<InternalValueType, const T2&>;
		template<typename T2>
		Array(const T2* ptr, uintMem count) requires IsArrayConstructibleFrom<InternalValueType, const T2&>;
		template<typename T2>
		Array(const std::initializer_list<T2>& arr) requires IsArrayConstructibleFrom<InternalValueType, const T2&>;
		template<typename T2>
		Array(const ArrayView<T2>& other) requires IsArrayConstructibleFrom<InternalValueType, const typename ArrayView<T2>::template InternalValueType&>;

		~Array();

		void Clear();
		bool Empty() const;
		uintMem Count() const;
		uintMem ReservedCount() const;

		template<typename ... Args>
		Iterator AddBack(Args&& ... args) requires std::constructible_from<InternalValueType, Args...> && std::move_constructible<InternalValueType>;
		/*
			This function will construct a new element only if the current buffer has space for it. If not returns
			a null iterator.
		*/
		template<typename ... Args>
		Iterator TryAddBack(Args&& ... args) requires std::constructible_from<InternalValueType, Args...>;

		template<typename ... Args>
		Iterator AddAt(uintMem index, Args&& ... args) requires std::constructible_from<InternalValueType, Args...> && std::move_constructible<InternalValueType>;
		template<typename ... Args>
		Iterator AddAt(Iterator it, Args&& ... args) requires std::constructible_from<InternalValueType, Args...> && std::move_constructible<InternalValueType>;

		//Inserts a array into the array, such that the object pointed by it will be directly after the inserted array
		void Insert(Iterator it, ArrayView<T> array) requires std::move_constructible<InternalValueType>;
		//Inserts a array into the array, such that the object at index 'index' will be directly after the inserted array
		void Insert(uintMem index, ArrayView<T> array) requires std::move_constructible<InternalValueType>;

		//Splits the array into two arrays. The elements from the first to 'it' would be in the current array and all elements
		//after and including 'it' will be returned.
		Array Split(Iterator it);
		//Splits the array into two arrays. The elements from the first to 'it' would be in the current array and all elements
		//after and including 'it' will be returned.
		Array Split(uintMem index);
		
		void EraseLast();		
		void EraseAt(uintMem index);
		void EraseAt(Iterator it);
		
		void Append(const Array& other) requires std::copy_constructible<InternalValueType>;
		void Append(Array&& other) requires std::move_constructible<InternalValueType>;
		
		template<typename ... Args> requires std::constructible_from<T, Args...>
		void Resize(uintMem newCount, Args&& ... args) requires std::move_constructible<InternalValueType>;
		template<typename F>
		void ResizeWithFunction(uintMem newCount, const F& constructFunction) requires std::invocable<F, InternalValueType*, uintMem> && std::move_constructible<InternalValueType>;
		
		/*
			Makes sure that the array buffer is big enough to support 'reserveCount' amount of elements. Destroys any
			already existing elements.
		*/
		void ReserveExactly(uintMem reserveCount);
		/*
			Makes sure that the array buffer is big enough to support additional 'reserveCount' amount of elements. The 
			count of current elements stays the same. Moves old elements to a new buffer if a new buffer is needed.
		*/
		void ReserveAdditional(uintMem reserveCount) requires std::constructible_from<T, T&&>;

		void Truncate(uintMem newCount);
	
		T& operator[](uintMem i);
		const T& operator[](uintMem i) const;
		
		InternalValueType* Ptr();
		const InternalValueType* Ptr() const;

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
		
		operator ArrayView<T>() const;		
		
		Array& operator=(const Array& other) requires std::assignable_from<InternalValueType, const InternalValueType&>;
		Array& operator=(Array&& other) noexcept;		
		template<typename T2, AllocatorType Allocator2>
		Array& operator=(const Array<T2, Allocator2>& arr) requires IsArrayAssignableFrom<InternalValueType, const typename Array<T2, Allocator2>::template InternalValueType&>;
		template<typename T2>
		Array& operator=(const ArrayView<T2>& arr) requires IsArrayAssignableFrom<InternalValueType, const typename ArrayView<T2>::template InternalValueType&>;

		template<typename>
		friend class ArrayIterator;
		template<typename, AllocatorType>
		friend class Array;
	private:				
		InternalValueType* ptr;
		uintMem count;
		uintMem reserved;
		BLAZE_ALLOCATOR_ATTRIBUTE Allocator allocator;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		uint32 iteratorCount;
#endif

		/*
			Constructs <count> elements by copy-constructing elements starting 
			at <src> without destroying previous elements. Changes the current 
			buffer pointer if needed: the old one is freed and a new one 
			allocated. 
		\param
			src - start of the source array to copy from
		\param
			count - number of elements to copy

		*/
		template<typename T2> requires IsArrayConstructibleFrom<T, const T2&>
		void CopyConstructUnsafe(const T2* src, uintMem count);
		/*
			If needed creates a new buffer to fit <count> elements in the new
			one, destroys the old elements, and copy-constructs all the elements
			from <src> to the new buffer. If no new buffer is needed existing
			elements are copy-assigned and excess in the current buffer are 
			destroyed or the excess in the source buffer are copy-constructed 
			into the current buffer.			
		\param
			src - start of the source array to copy from
		\param
			count - number of elements to copy

		*/
		template<typename T2> requires IsArrayAssignableFrom<T, const T2&>
		void CopyAssign(const T2* src, uintMem count);

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
		InternalValueType* ReallocateUnsafe(uintMem newCount);		
	};	

}