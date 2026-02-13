#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/Memory/Allocator.h"
#include <type_traits>
#include <concepts>

namespace Blaze
{
	template<typename T, AllocatorType Allocator>
	class List;

	template<typename T1, typename T2>
	concept IsConvertibleToListIterator =
		SameAs<RemoveConst<typename T2::ListType>, RemoveConst<typename T1::ListType>> &&
		(!(IsConstType<typename T1::ListType> && !IsConstType<typename T2::ListType>));

	template<typename T1, typename T2>
	concept IsComparableToListIterator =
		SameAs<RemoveConst<typename T2::ListType>, RemoveConst<typename T1::ListType>>;

	/*
		Used with the Blaze::List class.

		Iterator usage:
		Increasing, decreasing or dereferencing the iterator outside of valid range or invalidating the lists previous buffers while some iterators
		are referencing it cannot be detected if no debugging measures are taken. It is up to the client to ensure its valid use. A null list
		iterator is pointing to nullptr and therefore cannot be dereferenced, dereferencing it is considered an error (See BLAZE_NULL_ITERATOR_CHECK
		and BLAZE_CONTAINER_INVALIDATION_CHECK for further information about iterator invalidation).

		Null iterators can be created by the default constructor, by copying/assigning an null iterator, or increasing\decreasing the iterator
		outside the list. They cannot be dereferenced, incremented or decremented, it will result in an error or a possible silent fail depending
		if the BLAZE_NULL_ITERATOR_CHECK macro is defined.

		Erasing an element that no list iterator is referencing wont invalidate any list iterators.

		BLAZE_NULL_ITERATOR_CHECK:
		If BLAZE_NULL_ITERATOR_CHECK is defined, incrementing, decrementing or dereferencing a null list iterator will result in a fatal error.
		Otherwise it will try to continue execution and have a possible silent fail. Having this undefined will slightly improve performance.
		It IS defined in debug mode by default (it isn't defined in release mode).

		BLAZE_CONTAINER_INVALIDATION_CHECK:
		By changing a list element that an list iterator is pointing to (erasing, clearing, etc...) the iterator becomes invalid. If
		BLAZE_CONTAINER_INVALIDATION_CHECK is defined and any operation noted above is executed and there is a list iterator referencing the
		element, a fatal error will be logged. Otherwise it will try to continue execution and have a possible silent fail. When this macro
		is defined, the size of the nodes in the list change because additional information is needed to be stored for this macro to work.
		Having this undefined will improve performance. It is NOT defined in debug mode by default (not defined in release mode either).

		No other macros change the list iterator behaviour
	*/
	template<typename List>
	class BLAZE_API ListIterator
	{
	public:
		using ListType = List;
		using ValueType = Conditional<IsConstType<List>, const typename List::ValueType, typename List::ValueType>;

		//STD compatibility
		using value_type = ValueType;
		using difference_type = intMem;

		ListIterator();
		template<IsConvertibleToListIterator<ListIterator<List>> T>
		ListIterator(const T& i);

		bool IsNull() const;
		ValueType* Ptr() const;

		ListIterator& operator++();
		ListIterator operator++(int);

		ValueType& operator*() const;
		ValueType* operator->() const;

		template<IsComparableToListIterator<ListIterator<List>> T>
		bool operator==(const T& i) const;
		template<IsComparableToListIterator<ListIterator<List>> T>
		bool operator!=(const T& i) const;

		template<IsConvertibleToListIterator<ListIterator<List>> T>
		ListIterator& operator=(const T& i);

		template<typename, AllocatorType>
		friend class ::Blaze::List;
	private:
		using Node = Conditional<IsConstType<List>, const typename List::Node, typename List::Node>;

		Node* node;

		ListIterator(Node* node);
	};
	/*
		Basic list class

		BLAZE_INVALID_ITERATOR_CHECK:
		If BLAZE_INVALID_ITERATOR_CHECK is defined, passing an invalid iterator to a list will result in a fatal error. Having this
		undefined will slightly improve performance. It IS defined in debug mode by default (not defined in release mode).

		BLAZE_CONTAINER_INVALIDATION_CHECK:
		By changing a list element that an list iterator is pointing to (erasing, clearing, etc...) the iterator becomes invalid. If
		BLAZE_CONTAINER_INVALIDATION_CHECK is defined and any operation noted above is executed and there is a list iterator referencing the
		element, a fatal error will be logged. Otherwise it will try to continue execution and have a possible silent fail. When this macro
		is defined, the size of the nodes in the list change because additional information is needed to be stored for this macro to work.
		Having this undefined will improve performance. It is NOT defined in debug mode by default (not defined in release mode either).

		No other macros change the list behaviour
	*/
	template<typename T, AllocatorType Allocator = Blaze::DefaultAllocator>
	class BLAZE_API List
	{
	public:
		using Iterator = ListIterator<List>;
		using ConstIterator = ListIterator<const List>;
		using ValueType = T;
		using value_type = ValueType;

		List();
		List(const List& list) requires std::is_copy_constructible_v<T>;
		List(List&& list) noexcept;
		~List();

		void Clear();
		bool Empty() const;

		template<typename ... Args> requires IsConstructibleFrom<T, Args...>
		Iterator AddAfter(const Iterator& it, Args&& ... args);
		template<typename ... Args> requires IsConstructibleFrom<T, Args...>
		Iterator AddFront(Args&& ... args);

		void EraseAfter(const Iterator& it);
		void EraseFirst();
		template<typename C> requires std::invocable<C, T&> && SameAs<std::invoke_result_t<C, T&>, bool>
		void EraseAll(const C& function);
		template<typename C> requires std::invocable<C, T&>&& SameAs<std::invoke_result_t<C, T&>, bool>
		void EraseOne(const C& function);

		List SplitAfter(const Iterator& it);

		T& First();
		const T& First() const;

		/*
			Returns an iterator pointing to the first element in the list. If the list is empty returns a null list iterator.
		*/
		Iterator FirstIterator();
		/*
			Returns an iterator pointing to the first element in the list. If the list is empty returns a null list iterator.
		*/
		ConstIterator FirstIterator() const;
		/*
			Returns a null iterator always.
		*/
		Iterator BehindIterator();
		/*
			Returns a null iterator always.
		*/
		ConstIterator BehindIterator() const;

		List& operator=(const List& l) requires std::is_copy_assignable_v<T>;
		List& operator=(List&& l) noexcept;

		template<typename List>
		friend class ListIterator;
	private:
		struct Node
		{
			Node* next;
			ValueType value;
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
			uintMem iteratorCount;
#endif

			template<typename ... Args> requires IsConstructibleFrom<T, Args...>
			Node(Node* next, Args&& ... args);
		};

		//Wont free previous contents
		void CopyUnsafe(const List& other);

		Node* first;
		BLAZE_ALLOCATOR_ATTRIBUTE Allocator allocator;
	};

	template<typename T, AllocatorType Allocator>
	List<T, Allocator>::Iterator begin(List<T, Allocator>& list)
	{
		return list.FirstIterator();
	}
	template<typename T, AllocatorType Allocator>
	List<T, Allocator>::ConstIterator begin(const List<T, Allocator>& list)
	{
		return list.FirstIterator();
	}
	template<typename T, AllocatorType Allocator>
	List<T, Allocator>::Iterator end(List<T, Allocator>& list)
	{
		return list.BehindIterator();
	}
	template<typename T, AllocatorType Allocator>
	List<T, Allocator>::ConstIterator end(const List<T, Allocator>& list)
	{
		return list.BehindIterator();
	}
}

#include "BlazeEngine/Core/Container/ListImpl.h"