#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"
#include "BlazeEngineCore/Types/TypeTraits.h"
#include "BlazeEngineCore/Memory/Allocator.h"
#include "BlazeEngineCore/DataStructures/ArrayIterator.h"
#include <initializer_list>

namespace Blaze
{
	template<typename, AllocatorType>
	class Array;
	template<typename>
	class ArrayIterator;
	template<typename>
	class ArrayView;
	
	template<typename T, AllocatorType Allocator = DefaultAllocator> requires std::is_trivially_destructible_v<T>
	class BLAZE_CORE_API RawArray
	{
	public:
		using Iterator = ArrayIterator<T>;
		using ConstIterator = ArrayIterator<const T>;
		using ValueType = T;
		using StoredType = std::conditional_t<std::is_reference_v<T>, std::remove_reference_t<T>*, T>;
		using value_type = T;

		/*
			Constructs an empty array. The array will store a null pointer
		*/
		RawArray();		
		/*
			Move-constructs the array from another array
		*/
		RawArray(RawArray&& other) noexcept;
		/*
			Copy-constructs the array from a range limited by the begin and end iterators. The new array will
			copy each element individually. If either of the iterators in null an empty array is created. This
			constructor is only available if the value type is copy-constructible or is a reference type.
		*/
		RawArray(Iterator begin, Iterator end) requires std::copy_constructible<StoredType>;
		/*
			Constructs an array with 'count' elements. The elements are constructed by calling the
			'constructFunction' function. The function should take a pointer to the element and the index of the
			element. If the type is a reference type the function should take a pointer to a pointer to the
			element and the index of the element. The function should construct the value that the pointer is
			pointing to.
		*/
		template<typename F> requires std::invocable<F, typename RawArray<T, Allocator>::template StoredType*, uintMem>
		RawArray(uintMem count, const F& constructFunction);
		/*
			Constructs an array with 'count' elements. The elements are constructed by calling the constructor
			of the value type with the arguments 'args'. This constructor is only available if the value type is
			constructible from the arguments 'args' or the value type is a reference type and 'args' is a pointer
			to the type that is being referenced.
		*/
		template<typename ... Args> requires std::constructible_from<typename RawArray<T, Allocator>::template StoredType, Args...>
		RawArray(uintMem count, Args&& ... args);
		/*
			Constructs an array from a C-style array. Each element is copied individually. This constructor is
			only available if the value type is constructible from the type T2 or the value type is a reference
			type and T2 is a pointer to the type that is being referenced.
		*/
		template<typename T2, uintMem S> requires std::constructible_from<typename RawArray<T, Allocator>::template StoredType, const T2&>
		explicit RawArray(const T2(&arr)[S]);
		/*
			Constructs an array from an initializer list. Each element is copied individually. This constructor
			is only available if the value type is constructible from the type T2 or the value type is a
			reference type and T2 is a pointer to the type that is being referenced.
		*/
		template<typename T2> requires std::constructible_from<typename RawArray<T, Allocator>::template StoredType, const T2&>
		explicit RawArray(const std::initializer_list<T2>& arr);
		/*
			Constructs an array from a pointer to a range of elements. Each element is copied individually.	If
			the ptr is nullptr and count is non-zero, 'count' elements are created and default-constructed. If
			both ptr and count are zero an empty array is created. If the count is 0 the pointer must be 0. This
			constructor is only available if the value type is constructible from the type T2 or the value type
			is a reference type and T2 is a pointer to the type that is being referenced.
		*/
		template<typename T2> requires std::constructible_from<typename RawArray<T, Allocator>::template StoredType, const T2&>
		RawArray(const T2* ptr, uintMem count);
		/*
			Constructs an array by converting elements from another array. This constructor is only available if
			the value type is constructible from the type T2 or the value type is a reference type and T2 is a
			pointer to the type that is being referenced.
		*/
		template<typename T2, AllocatorType Allocator2> requires std::constructible_from<typename Array<T, Allocator>::template StoredType, const T2&>
		explicit RawArray(const Array<T2, Allocator2>& other);
		/*
			Constructs an array by converting elements from another array view. This constructor is only
			available if the value type is constructible from the type T2 or the value type is a reference type
			and T2 is a pointer to the type that is being referenced.
		*/
		template<typename T2> requires std::constructible_from<typename Array<T, Allocator>::template StoredType, const T2&>
		explicit RawArray(const ArrayView<T2>& other);

		~RawArray();

		/*
			Destroys all the elements in the array and frees the memory. The array will have a null pointer
		*/
		void Clear();
		/*
			\return true if the array is empty, otherwise returns false.
		*/
		bool Empty() const;
		/*
			Resize the array to the given count. The old array is cleared
		*/
		template<typename ... Args> requires std::constructible_from<typename RawArray<T, Allocator>::template StoredType, Args...>
		void Reallocate(uintMem count, Args&& ... args);

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
			\return An iterator pointing one element behind the array. If the array is empty returns a null array iterator.
		*/
		Iterator AheadIterator();
		/*
			\return An iterator pointing one element ahead the array. If the array is empty returns a null array iterator.
		*/
		ConstIterator AheadIterator() const;
		
		/*
			Move-assigns the array from an another array.
		*/
		RawArray& operator=(RawArray&& arr) noexcept;
		/*
			Copy-assigns the array from another array. The new array will convert each element individually.

			\param other - the array to copy from
			\return Reference to the current array
		*/
		template<typename T2, AllocatorType Allocator2> requires std::constructible_from<typename Array<T, Allocator>::template StoredType, const T2&>&& std::assignable_from<typename Array<T, Allocator>::template StoredType&, const T2&>
		RawArray& operator=(const Array<T2, Allocator2>& arr);
		/*
			Copy-assigns the array from another array view. The new array will convert each element individually.

			\param other - the array to copy from
			\return Reference to the current array
		*/
		template<typename T2> requires std::constructible_from<typename Array<T, Allocator>::template StoredType, const T2&>&& std::assignable_from<typename Array<T, Allocator>::template StoredType&, const T2&>
		RawArray& operator=(const ArrayView<T2>& arr);
	private:
		StoredType* ptr;		
		BLAZE_ALLOCATOR_ATTRIBUTE Allocator allocator;
	};

	template<typename T, AllocatorType Allocator>
	auto begin(RawArray<T, Allocator>& arr)
	{
		return arr.FirstIterator();
	}
	template<typename T, AllocatorType Allocator>
	auto begin(const RawArray<T, Allocator>& arr)
	{
		return arr.FirstIterator();
	}
	template<typename T, AllocatorType Allocator>
	auto end(RawArray<T, Allocator>& arr)
	{
		return arr.BehindIterator();
	}
	template<typename T, AllocatorType Allocator>
	auto end(const RawArray<T, Allocator>& arr)
	{
		return arr.BehindIterator();
	}				
}

#include "RawArrayImpl.h"