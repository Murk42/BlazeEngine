#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/Debug/Logger.h"

namespace Blaze
{
	template<typename T, AllocatorType Allocator>
	class DualList;

	template<typename T1, typename T2>
	concept IsConvertibleToDualListIterator =
		SameAs<RemoveConst<typename T2::DualListType>, RemoveConst<typename T1::DualListType>> &&
		(!(IsConstType<typename T1::DualListType> && !IsConstType<typename T2::DualListType>));

	template<typename T1, typename T2>
	concept IsComparableToDualListIterator =
		SameAs<RemoveConst<typename T2::DualListType>, RemoveConst<typename T1::DualListType>>;

	/*
		Used with the Blaze::DualList class.

		Iterator usage:
		Increasing, decreasing or dereferencing the iterator outside of valid range or invalidating the dual lists previous buffers while some iterators
		are referencing it cannot be detected if no debugging measures are taken. It is up to the client to ensure its valid use. A null dual list
		iterator is pointing to nullptr and therefore cannot be dereferenced, dereferencing it is considered an error (See BLAZE_NULL_ITERATOR_CHECK
		and BLAZE_CONTAINER_INVALIDATION_CHECK for further information about iterator invalidation).

		Null iterators can be created by the default constructor, by copying/assigning an null iterator, or increasing\decreasing the iterator
		outside the dual list. They cannot be dereferenced, incremented or decremented, it will result in an error or a possible silent fail depending
		if the BLAZE_NULL_ITERATOR_CHECK macro is defined.

		Erasing an element that no dual list iterator is referencing wont invalidate any dual list iterators.

		BLAZE_NULL_ITERATOR_CHECK:
		If BLAZE_NULL_ITERATOR_CHECK is defined, incrementing, decrementing or dereferencing a null dual list iterator will result in a fatal error.
		Otherwise it will try to continue execution and have a possible silent fail. Having this undefined will slightly improve performance.
		It IS defined in debug mode by default (it isn't defined in release mode).

		BLAZE_CONTAINER_INVALIDATION_CHECK:
		By changing a dual list element that an dual list iterator is pointing to (erasing, clearing, etc...) the iterator becomes invalid. If
		BLAZE_CONTAINER_INVALIDATION_CHECK is defined and any operation noted above is executed and there is a dual list iterator referencing the
		element, a fatal error will be logged. Otherwise it will try to continue execution and have a possible silent fail. When this macro
		is defined, the size of the nodes in the dual list change because additional information is needed to be stored for this macro to work.
		Having this undefined will improve performance. It is NOT defined in debug mode by default (not defined in release mode either).

		No other macros change the dual list iterator behavior
	*/
	template<typename DualList>
	class BLAZE_API DualListIterator
	{
	public:
		using DualListType = DualList;
		using ValueType = Conditional<IsConstType<DualList>, const typename DualList::ValueType, typename DualList::ValueType>;

		//STD compatibility
		using value_type = ValueType;
		using difference_type = intMem;

		DualListIterator();
		template<IsConvertibleToDualListIterator<DualListIterator<DualList>> T>
		DualListIterator(const T& i);

		bool IsNull() const;

		DualListIterator& operator++();
		DualListIterator operator++(int);
		DualListIterator& operator--();
		DualListIterator operator--(int);

		ValueType& operator*() const;
		ValueType* operator->() const;

		template<IsComparableToDualListIterator<DualListIterator<DualList>> T>
		bool operator==(const T& i) const;
		template<IsComparableToDualListIterator<DualListIterator<DualList>> T>
		bool operator!=(const T& i) const;

		template<IsConvertibleToDualListIterator<DualListIterator<DualList>> T>
		DualListIterator& operator=(const T& i);

		template<typename, AllocatorType>
		friend class ::Blaze::DualList;
		template<typename>
		friend class ::Blaze::DualListIterator;
	private:
		using Node = typename DualList::Node;

		Node* node;

		DualListIterator(Node* node);
	};

	/*
		Basic dual list class

		BLAZE_INVALID_ITERATOR_CHECK:
		If BLAZE_INVALID_ITERATOR_CHECK is defined, passing an invalid iterator to a dual list will result in a fatal error. Having this
		undefined will slightly improve performance. It IS defined in debug mode by default (not defined in release mode).

		BLAZE_CONTAINER_INVALIDATION_CHECK:
		By changing a dual list element that an dual list iterator is pointing to (erasing, clearing, etc...) the iterator becomes invalid. If
		BLAZE_CONTAINER_INVALIDATION_CHECK is defined and any operation noted above is executed and there is a dual list iterator referencing the
		element, a fatal error will be logged. Otherwise it will try to continue execution and have a possible silent fail. When this macro
		is defined, the size of the nodes in the dual list change because additional information is needed to be stored for this macro to work.
		Having this undefined will improve performance. It is NOT defined in debug mode by default (not defined in release mode either).

		No other macros change the dual list behaviour
	*/
	template<typename T, AllocatorType Allocator = Blaze::DefaultAllocator>
	class BLAZE_API DualList : private Allocator
	{
	public:
		using Iterator = DualListIterator<DualList>;
		using ConstIterator = DualListIterator<const DualList>;
		using ValueType = T;

		DualList();
		DualList(const DualList& list);
		DualList(DualList&& list) noexcept;
		~DualList();

		void Clear();
		bool Empty() const;
		uintMem Count() const;

		template<typename ... Args> requires IsConstructibleFrom<T, Args...>
		Iterator AddBefore(const Iterator& it, Args&& ... args);
		template<typename ... Args> requires IsConstructibleFrom<T, Args...>
		Iterator AddAfter(const Iterator& it, Args&& ... args);
		template<typename ... Args> requires IsConstructibleFrom<T, Args...>
		Iterator AddBack(Args&& ... args);
		template<typename ... Args> requires IsConstructibleFrom<T, Args...>
		Iterator AddFront(Args&& ... args);

		void Erase(const Iterator& it);
		template<typename C> requires std::invocable<C, const T&>&& SameAs<std::invoke_result_t<C, const T&>, bool>
		void EraseAll(const C& function);
		template<typename C> requires std::invocable<C, const T&>&& SameAs<std::invoke_result_t<C, const T&>, bool>
		void EraseOne(const C& function);
		void EraseFirst();
		void EraseLast();

		void MoveBefore(const ConstIterator& iterator, const ConstIterator& relative);
		void MoveAfter(const ConstIterator& iterator, const ConstIterator& relative);
		void MoveFront(const ConstIterator& iterator);
		void MoveBack(const ConstIterator& iterator);

		void AppendBack(const DualList& list);
		void AppendBack(DualList&& list);
		void AppendFront(const DualList& list);
		void AppendFront(DualList&& list);

		T& First();
		const T& First() const;
		T& Last();
		const T& Last() const;

		/*
			Returns an iterator pointing to the first element in the list. If the list is empty returns a null list iterator.
		*/
		Iterator FirstIterator();
		/*
			Returns an iterator pointing to the first element in the list. If the list is empty returns a null list iterator.
		*/
		ConstIterator FirstIterator() const;
		/*
			Returns an iterator pointing to the last element in the list. If the list is empty returns a null list iterator.
		*/
		Iterator LastIterator();
		/*
			Returns an iterator pointing to the last element in the list. If the list is empty returns a null list iterator.
		*/
		ConstIterator LastIterator() const;
		/*
			Returns a null iterator always.
		*/
		Iterator AheadIterator();
		/*
			Returns a null iterator always.
		*/
		ConstIterator AheadIterator() const;
		/*
			Returns a null iterator always.
		*/
		Iterator BehindIterator();
		/*
			Returns a null iterator always.
		*/
		ConstIterator BehindIterator() const;

		DualList& operator=(const DualList& l);
		DualList& operator=(DualList&& l) noexcept;

		template<typename DualList>
		friend class DualListIterator;
	private:
		struct Node
		{
			Node* next;
			Node* prev;
			ValueType value;
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
			uint iteratorCount;
#endif

			template<typename ... Args> requires IsConstructibleFrom<T, Args...>
			Node(Node* prev, Node* next, Args&& ... args);
		};

		//Wont free previous contents
		void CopyUnsafe(const DualList& other);
		//Wont check if bucket or node is nullptr
		static void RemoveNodeFromSpotUnsafe(Node* node);

		Node* first;
		Node* last;
		size_t count;
		BLAZE_ALLOCATOR_ATTRIBUTE Allocator allocator;
	};

	template<typename T, AllocatorType Allocator>
	DualList<T, Allocator>::Iterator begin(DualList<T, Allocator>& list)
	{
		return list.FirstIterator();
	}
	template<typename T, AllocatorType Allocator>
	DualList<T, Allocator>::ConstIterator begin(const DualList<T, Allocator>& list)
	{
		return list.FirstIterator();
	}
	template<typename T, AllocatorType Allocator>
	DualList<T, Allocator>::Iterator end(DualList<T, Allocator>& list)
	{
		return list.BehindIterator();
	}
	template<typename T, AllocatorType Allocator>
	DualList<T, Allocator>::ConstIterator end(const DualList<T, Allocator>& list)
	{
		return list.BehindIterator();
	}
}

#include "BlazeEngine/Core/Container/DualListImpl.h"