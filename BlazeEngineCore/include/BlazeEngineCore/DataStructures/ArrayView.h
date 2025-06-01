#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"
#include "BlazeEngineCore/Types/TypeTraits.h"
#include "BlazeEngineCore/Memory/Allocator.h"
#include "BlazeEngineCore/DataStructures/ArrayIterator.h"
#include <initializer_list>

namespace Blaze
{		
	template<typename T>
	class ArrayIterator;

	template<typename, AllocatorType>
	class Array;

	template<typename T>
	class BLAZE_CORE_API ArrayView
	{			
	public:
		using Iterator = ArrayIterator<const T>;		
		using ValueType = T;				
		using StoredType = std::conditional_t<std::is_reference_v<T>, std::remove_reference_t<T>* const, T>;
		using value_type = T;

		constexpr ArrayView();
		constexpr ArrayView(const ArrayView&);
		constexpr ArrayView(Iterator begin, Iterator end);
		template<uintMem S>
		constexpr ArrayView(const StoredType(&arr)[S]);
		constexpr ArrayView(const std::initializer_list<StoredType>& list);
		constexpr ArrayView(const StoredType* ptr, uintMem count);

		constexpr void Clear();
		constexpr bool Empty() const;
		constexpr uintMem Count() const;

		constexpr const T& operator[](uintMem index) const;

		constexpr const StoredType* Ptr() const;
		
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

		/*
			If T is a reference type, true is returned if all of the references in this array refer to the same object as the coresponding reference in the second array, otherwise returnes false
		*/
		constexpr bool operator==(const ArrayView<T>& other) const requires std::equality_comparable<T> || std::is_reference_v<T>;
		/*
			If T is a reference type, false is returned if all of the references in this array refer to the same object as the coresponding reference in the second array, otherwise returnes true
		*/
		constexpr bool operator!=(const ArrayView<T>& other) const requires std::equality_comparable<T> || std::is_reference_v<T>;

		constexpr ArrayView& operator=(const ArrayView& array);
		template<AllocatorType Allocator>
		constexpr ArrayView& operator=(const Array<T, Allocator>& array);		
	private:		
		const StoredType* ptr;
		uintMem count;
	};			

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