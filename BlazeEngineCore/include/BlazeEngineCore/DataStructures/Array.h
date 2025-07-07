#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"
#include "BlazeEngineCore/Types/TypeTraits.h"
#include "BlazeEngineCore/Memory/Allocator.h"
#include "BlazeEngineCore/DataStructures/ArrayIterator.h"
#include <initializer_list>

namespace Blaze
{
	template<typename>
	class ArrayIterator;
	template<typename>
	class ArrayView;			

	/*
		Basic array class		

		BLAZE_INVALID_ITERATOR_CHECK:
		If BLAZE_INVALID_ITERATOR_CHECK is defined, passing an invalid iterator or index to a array will result in a fatal error. Having this 
		undefined will slightly improve performance. It IS defined in debug mode by default (not defined in release mode).		

		No other macros change the array behaviour
	*/
	template<typename T, AllocatorType Allocator = DefaultAllocator>
	class BLAZE_CORE_API Array
	{						
	public:
		using Iterator = ArrayIterator<T>;
		using ConstIterator = ArrayIterator<const T>;
		using ValueType = T;				
		using StoredType = std::conditional_t<std::is_reference_v<T>, std::remove_reference_t<T>*, T>;
		using value_type = T;

		/*
			Constructs an empty array. The array will have a null pointer and a count of 0.
		*/
		Array();
		/*
			Copy-constructs the array from another array. The new array will copy each element individually. 
			This constructor is only available if the value type is copy-constructible or is a reference type.
		*/
		Array(const Array& other) requires std::copy_constructible<StoredType>;
		/*
			Move-constructs the array from another array
		*/
		Array(Array&& other) noexcept;
		/*
			Copy-constructs the array from another array view. The new array will copy each element individually. 
			This constructor is only available if the value type is copy-constructible or is a reference type.
		*/
		Array(const ArrayView<T>& other) requires std::copy_constructible<StoredType>;
		/*
			Copy-constructs the array from a range limited by the begin and end iterators. The new array will 
			copy each element individually. If either of the iterators in null an empty array is created. This 
			constructor is only available if the value type is copy-constructible or is a reference type.
		*/
		Array(Iterator begin, Iterator end) requires std::copy_constructible<StoredType>;
		/*
			Constructs an array with 'count' elements. The elements are constructed by calling the 
			'constructFunction' function. The function should take a pointer to the element and the index of the
			element. If the type is a reference type the function should take a pointer to a pointer to the 
			element and the index of the element. The function should construct the value that the pointer is
			pointing to.
		*/
		template<typename F> requires std::invocable<F, typename Array<T, Allocator>::template StoredType*, uintMem>
		Array(uintMem count, const F& constructFunction);
		/*
			Constructs an array with 'count' elements. The elements are constructed by calling the constructor
			of the value type with the arguments 'args'. This constructor is only available if the value type is
			constructible from the arguments 'args' or the value type is a reference type and 'args' is a pointer
			to the type that is being referenced.
		*/
		template<typename ... Args> requires std::constructible_from<typename Array<T, Allocator>::template StoredType, Args...>
		Array(uintMem count, Args&& ... args);		 
		/*
			Constructs an array from a C-style array. Each element is copied individually. This constructor is 
			only available if the value type is constructible from the type T2 or the value type is a reference 
			type and T2 is a pointer to the type that is being referenced.
		*/
        template<typename T2, uintMem S> requires std::constructible_from<typename Array<T, Allocator>::template StoredType, const T2&>
        explicit Array(const T2(&arr)[S]);
		/*
			Constructs an array from an initializer list. Each element is copied individually. This constructor 
			is only available if the value type is constructible from the type T2 or the value type is a 
			reference type and T2 is a pointer to the type that is being referenced.
		*/
        template<typename T2> requires std::constructible_from<typename Array<T, Allocator>::template StoredType, const T2&>
        explicit Array(const std::initializer_list<T2>& arr);
		/*
			Constructs an array from a pointer to a range of elements. Each element is copied individually.	If 
			the ptr is nullptr and count is non-zero, 'count' elements are created and default-constructed. If 
			both ptr and count are zero an empty array is created. If the count is 0 the pointer must be 0. This
			constructor is only available if the value type is constructible from the type T2 or the value type 
			is a reference type and T2 is a pointer to the type that is being referenced.
		*/
        template<typename T2> requires std::constructible_from<typename Array<T, Allocator>::template StoredType, const T2&>
        Array(const T2* ptr, uintMem count);
		/*
			Constructs an array by converting elements from another array. This constructor is only available if 
			the value type is constructible from the type T2 or the value type is a reference type and T2 is a 
			pointer to the type that is being referenced.			
		*/
        template<typename T2, AllocatorType Allocator2> requires std::constructible_from<typename Array<T, Allocator>::template StoredType, const T2&>
        explicit Array(const Array<T2, Allocator2>& other);
		/*
			Constructs an array by converting elements from another array view. This constructor is only 
			available if the value type is constructible from the type T2 or the value type is a reference type 
			and T2 is a pointer to the type that is being referenced.
		*/
        template<typename T2> requires std::constructible_from<typename Array<T, Allocator>::template StoredType, const T2&>
        explicit Array(const ArrayView<T2>& other);

		~Array();

		/*
			Destroys all the elements in the array and frees the memory. The array will have a null pointer and 
			a count of 0.
		*/
		void Clear();
		/*
			\return true if the array is empty, otherwise returns false.
		*/
		bool Empty() const;
		/*
			\return The count of elements
		*/
		uintMem Count() const;
		/*
			\return Count of how many elements can the internal buffer hold without reallocation
		*/
		uintMem ReservedCount() const;

		/*
			Adds a element to the end of the array and constructs it using 'args', returns a iterator to the 
			added element. This function is avaiable if the type is move constructible and the value type must 
			be constructible from the arguments 'args' or the value type is a reference type and 'args' is a pointer to the type that 
			is being referenced.
			
			\param args - arguments to construct the element with			
			\return Iterator pointing to the added element
		*/
		template<typename ... Args> requires std::constructible_from<T, Args...>
		Iterator AddBack(Args&& ... args) requires std::move_constructible<StoredType>;		
		/*
			Ties to add a element to the end of the array and constructs it using 'args', if the internal buffer
			has to be resized nothing will happen and a null iterator is returned, otherwise a iterator to the 
			added element is returned. This is usefull if you know there is enoguh space because you dont have a 
			constraint that the type is move-constructible This function is avaiable if the type is 
			move-constructible and the value type must be constructible from the arguments 'args' or the value 
			type is a reference type and 'args' is a pointer to the type that is being referenced.
			
			\param args - arguments to construct the element with
			\return Iterator pointing to the added element, or null if no element is added
		*/
		template<typename ... Args> requires std::constructible_from<T, Args...>
		Iterator TryAddBack(Args&& ... args);		
		/*
			Adds a element before 'index'-th element and constructs it using 'args', returns a iterator to the
			added element. This function is avaiable if the type is move constructible and is constructible from
			the arguments 'args' or the value type is a reference type and 'args' is a pointer to the type that
			is being referenced.

			\param index - index of the element before which to add the element
			\param args - arguments to construct the element with
			\return Iterator pointing to the added element
		*/
		template<typename ... Args> requires std::constructible_from<T, Args...>
		Iterator AddAt(uintMem index, Args&& ... args) requires std::move_constructible<StoredType>;		
		/*
			Adds a element before the element pointed by 'it' and constructs it using 'args', returns a iterator 
			to the added element. This function is avaiable if the type is move constructible and is 
			constructible from the arguments 'args' or the value type is a reference type and 'args' is a 
			pointer to the type that is being referenced.

			\param it - iterator pointing to the element before which to add the element
			\param args - arguments to construct the element with
			\return Iterator pointing to the added element
		*/
		template<typename ... Args> requires std::constructible_from<T, Args...>
		Iterator AddAt(Iterator it, Args&& ... args) requires std::move_constructible<StoredType>;		

		
		/*
			Inserts an array into the array before the elemenet with index 'index'.

			\param index - index of the element before which to insert the other array
			\param array - array to be inserted
		*/
		void Insert(uintMem index, ArrayView<T> array) requires std::move_constructible<StoredType>;
		/*
			Inserts an array into the array before the elemenet pointed by 'it'.

			\param it - iterator pointing to the element before which to insert the other array
			\param array - array to be inserted
		*/
		void Insert(Iterator it, ArrayView<T> array) requires std::move_constructible<StoredType>;

		/*
			Splits the array into two arrays. The elements from the first to 'it' would be in the current array and all elements
			after and including 'it' will be returned.

			\param it - iterator pointing to the element before which to split the array
			\return Array containing all elements after and including 'it'
		*/		
		Array Split(Iterator it);
		/*
			Splits the array into two arrays. The elements from the first to 'it' would be in the current array and all elements
			after and including 'it' will be returned.

			\param index - index of the element before which to split the array
			\return Array containing all elements after and including 'it'
		*/
		Array Split(uintMem index);
				
		/*		
			Removes the last element from the array. The array mustn't be empty
		*/
		void EraseLast() requires std::is_move_constructible_v<StoredType>;
		/*
			Removes the element with index 'index'. The array mustn't be empty

			\param index - index of the element to remove
		*/
		void EraseAt(uintMem index) requires std::is_move_assignable_v<StoredType> || std::is_move_constructible_v<StoredType>;
		/*
			Removes the element pointed by 'it'. The array mustn't be empty

			\param it - iterator pointing to the element to remove
		*/
		void EraseAt(Iterator it) requires std::is_move_assignable_v<StoredType> || std::is_move_constructible_v<StoredType>;
		
		/*
			Appends all elements from 'other' to the end of the array. The elements are copied individually
		*/
		void Append(const Array& other) requires std::copy_constructible<StoredType>;
		/*
			Appends all elements from 'other' to the end of the array. The elements are moved individually
		*/
		void Append(Array&& other) requires std::move_constructible<StoredType>;
		
		/*
			Resizes the array to 'newCount' elements. If the new count is smaller than the current count the 
			elements are destroyed. If the new count is bigger the elements are constructed from 'args'. This 
			function is available only if the value type move-constructible and is constructible from the 
			arguments 'args' or the value type is a reference type and 'args' is a pointer to the type that is 
			being referenced.

			\param newCount - the new count of elements
			\param args - arguments to construct the elements with
		*/
		template<typename ... Args> requires std::constructible_from<T, Args...>
		void Resize(uintMem newCount, Args&& ... args) requires std::move_constructible<StoredType>;
		/*
			Resizes the array to 'newCount' elements. If the new count is smaller than the current count the
			elements are destroyed. If the new count is bigger the elements are constructed by calling the 
			'constructingFunction' function, it should take a pointer to the element and the index of the
			element, but if the value type is a reference type the function should take a pointer to a pointer 
			to the element and the index of the element. The function should construct the value that the 
			pointer is pointing to. This function is available only if the value type is move-constructible.

			\param newCount - the new count of elements
			\param constructingFunction - function to construct the elements
		*/
		template<typename F> requires std::invocable<F, typename Array<T, Allocator>::StoredType*, uintMem>
		void ResizeWithFunction(uintMem newCount, const F& constructingFunction) requires std::move_constructible<StoredType>;
		
		/*
			Makes sure that the array buffer is big enough to support 'reserveCount' amount of elements. 
			Destroys any already existing elements.

			\param reserveCount - the count of elements that the array should be able to hold without 
			allocating new memory
		*/
		void ReserveExactly(uintMem reserveCount);
		/*
			Makes sure that the array buffer is big enough to support additional 'reserveCount' amount of elements. The 
			count of current elements stays the same. Moves old elements to a new buffer if a new buffer is needed.			

			\param reserveCount - the count of elements that the array should be able to hold without
			allocating new memory
		*/
		void ReserveAdditional(uintMem reserveCount) requires std::constructible_from<T, T&&>;
		/*
			If the count of elements is bigger than 'newCount' the array is resized to 'newCount' elements, otherwise
			does nothing.

			\param newCount - the maximum count of elements that the array should have
		*/
		void Truncate(uintMem newCount);
	
		/*
			\return A reference to the element with index 'i'. The array mustn't be empty
		*/
		T& operator[](uintMem i);
		/*
			\return A reference to the element with index 'i'. The array mustn't be empty
		*/
		const T& operator[](uintMem i) const;
		
		/*
			\return A pointer to the buffer of stored elements
		*/
		StoredType* Ptr();
		/*
			\return A pointer to the buffer of stored elements
		*/
		const StoredType* Ptr() const;
		
		/*
			\return A reference to the first element in the array. The array mustn't be empty
		*/
		T& First();
		/*
			\return A reference to the first element in the array. The array mustn't be empty
		*/
		const T& First() const;
		/*
			\return A reference to the last element in the array. The array mustn't be empty
		*/
		T& Last();
		/*
			\return A reference to the last element in the array. The array mustn't be empty
		*/		
		const T& Last() const;

		/*
			\param index - index of the element whose iterator to get
			\return An iterator pointing to the element with index 'index'. The index must be within the array 
			bounds.
		*/
		Iterator GetIterator(uintMem index);
		/*
			\param index - index of the element whose iterator to get
			\return An iterator pointing to the element with index 'index'. The index must be within the array
			bounds.
		*/
		ConstIterator GetIterator(uintMem index) const;

		/*
			\return An iterator pointing to the first element in the array. If the array is empty returns a null array iterator.
		*/
		Iterator FirstIterator();
		/*
			\return An iterator pointing to the first element in the array. If the array is empty returns a null array iterator.
		*/
		ConstIterator FirstIterator() const;
		/*
			\return An iterator pointing to the last element in the array. If the array is empty returns a null array iterator.
		*/
		Iterator LastIterator();
		/*
			\return An iterator pointing to the last element in the array. If the array is empty returns a null array iterator.
		*/
		ConstIterator LastIterator() const;
		/*
			\return An iterator pointing one element behind the array. If the array is empty returns a null array iterator.
		*/
		Iterator AheadIterator();
		/*
			\return An iterator pointing one element ahead the array. If the array is empty returns a null array iterator.
		*/
		ConstIterator AheadIterator() const;
		/*
			\return An iterator pointing one element behind the array. If the array is empty returns a null array iterator.
		*/
		Iterator BehindIterator();
		/*
			\return An iterator pointing one element behind the array. If the array is empty returns a null array iterator.
		*/
		ConstIterator BehindIterator() const;
		
		/*
			\return A view of the array
		*/
		operator ArrayView<T>() const;		
		
		/*
			Compares the arrays. This function is only available if the value type is comparable, or if the 
			value type is a reference type.

			\param other - the array to compare to
			\return True if both array element counts are the same and all elements are equal, or if the value 
			type is a reference type if all the corresponding elements point to the same object, otherwise 
			returns false
		*/
		bool operator==(const ArrayView<T>& other) const requires std::equality_comparable<StoredType>;
		/*
			Compares the arrays. This function is only available if the value type is comparable, or if the
			value type is a reference type.

			\param other - the array to compare to
			\return False if both array element counts are the same and all elements are equal, or if the value 
			type is a reference type if all the corresponding elements point to the same object, otherwise 
			returns false
		*/
		bool operator!=(const ArrayView<T>& other) const requires std::equality_comparable<StoredType>;

		/*
			Copy-assigns the array from another array. The new array will copy each element individually.
			This function is only available if the value type is copy-constructible and copy-assignableor is a 
			reference type.

			\param other - the array to copy from
			\return Reference to the current array
		*/
		Array& operator=(const Array& other) requires std::copy_constructible<StoredType> && std::assignable_from<StoredType&, const StoredType&>;
		/*
			Move-assigns the array from another array

			\param other - the array to move from
			\return Reference to the current array
		*/
		Array& operator=(Array&& other) noexcept;		
		/*
			Copy-assigns the array from another array. The new array will convert each element individually.

			\param other - the array to copy from
			\return Reference to the current array
		*/
		template<typename T2, AllocatorType Allocator2> requires std::constructible_from<typename Array<T, Allocator>::template StoredType, const T2&>&& std::assignable_from<typename Array<T, Allocator>::template StoredType& , const T2&>
		Array& operator=(const Array<T2, Allocator2>& arr);
		/*
			Copy-assigns the array from another array view. The new array will convert each element individually.

			\param other - the array to copy from
			\return Reference to the current array
		*/
		template<typename T2> requires std::constructible_from<typename Array<T, Allocator>::template StoredType, const T2&>&& std::assignable_from<typename Array<T, Allocator>::template StoredType&, const T2&>
		Array& operator=(const ArrayView<T2>& arr);
		
		template<typename, AllocatorType>
		friend class Array;
	private:						
		StoredType* ptr;
		uintMem count;
		uintMem reserved;
		BLAZE_ALLOCATOR_ATTRIBUTE Allocator allocator;

		/*
			Constructs <count> elements by copy-constructing elements starting 
			at <src> without destroying previous elements. Changes the current 
			buffer pointer if needed: the old one is freed and a new one 
			allocated.

			\param src - start of the source array to copy from. Mustn't be nullptr
			\param count - number of elements to copy
		*/
		template<typename T2> requires std::constructible_from<typename Array<T, Allocator>::template StoredType, const T2&>
		void CopyConstructUnsafe(const T2* src, uintMem count);
		/*
			If needed creates a new buffer to fit <count> elements in the new
			one, destroys the old elements, and copy-constructs all the elements
			from <src> to the new buffer. If no new buffer is needed existing
			elements are copy-assigned and excess in the current buffer are 
			destroyed or the excess in the source buffer are copy-constructed 
			into the current buffer.			

			\param src - start of the source array to copy from
			\param count - number of elements to copy
		*/
		template<typename T2> requires std::constructible_from<typename Array<T, Allocator>::template StoredType, const T2&>&& std::assignable_from<typename Array<T, Allocator>::template StoredType&, const T2&>
		void CopyAssign(const T2* src, uintMem count);

		/*
		
			Allocates a new buffer if the old one needs to be changed, returns nullptr
			otherwise. Only changes the 'reserved' variable. Doesn't move, copy or 
			destroy anything in the old or the new buffer, just allocates the memory.
			Doesn't free the old buffer.
		
			\param newCount - number of elements that will be able to fit in the new buffer
			\returns A pointer to a new buffer if the old one needs to be changed. Nullptr otherwise.
		*/
		StoredType* ReallocateUnsafe(uintMem newCount);		
	};	

	template<typename T, AllocatorType Allocator>
	auto begin(Array<T, Allocator>& arr)
	{
		return arr.FirstIterator();
	}
	template<typename T, AllocatorType Allocator>
	auto begin(const Array<T, Allocator>& arr)
	{
		return arr.FirstIterator();
	}
	template<typename T, AllocatorType Allocator>
	auto end(Array<T, Allocator>& arr)
	{
		return arr.BehindIterator();
	}
	template<typename T, AllocatorType Allocator>
	auto end(const Array<T, Allocator>& arr)
	{
		return arr.BehindIterator();
	}
}