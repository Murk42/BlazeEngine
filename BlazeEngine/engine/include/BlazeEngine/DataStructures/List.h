#pragma once

namespace Blaze
{			
	template<typename T, AllocatorType Allocator>
	class List;

	template<typename T1, typename T2>
	concept IsConvertibleToListIterator =
		std::same_as<std::remove_const_t<typename T2::template ListType>, std::remove_const_t<typename T1::template ListType>> &&
		(!(std::is_const_v<typename T1::template ListType> && !std::is_const_v<typename T2::template ListType>));

	template<typename T1, typename T2>
	concept IsComparableToListIterator =
		std::same_as<std::remove_const_t<typename T2::template ListType>, std::remove_const_t<typename T1::template ListType>>;

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
		using ValueType = std::conditional_t<std::is_const_v<List>, const typename List::template ValueType, typename List::template ValueType>;
		using value_type = ValueType;

		ListIterator();
		template<IsConvertibleToListIterator<ListIterator<List>> T>
		ListIterator(const T& i);		
		
		bool IsNull() const;		

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
		using Node = ::std::conditional_t<std::is_const_v<List>, const typename List::template Node, typename List::template Node>;

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
		uint Count() const;

		template<typename ... Args> requires std::constructible_from<T, Args...>
		Iterator AddAfter(const Iterator& it, Args&& ... args);
		template<typename ... Args> requires std::constructible_from<T, Args...>
		Iterator AddBack(Args&& ... args);
		template<typename ... Args> requires std::constructible_from<T, Args...>
		Iterator AddFront(Args&& ... args);

		Result EraseAfter(const Iterator& it);
		Result EraseFirst();		
		template<typename C> requires std::invocable<C, T&> && std::same_as<std::invoke_result_t<C, T&>, bool>
		Result EraseAll(const C& function);
		template<typename C> requires std::invocable<C, T&>&& std::same_as<std::invoke_result_t<C, T&>, bool>
		Result EraseOne(const C& function);

		void AppendBack(const List& list);
		void AppendBack(List&& list);
		void AppendFront(const List& list);
		void AppendFront(List&& list);		

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

		List& operator=(const List& l) requires std::is_copy_assignable_v<T>;
		List& operator=(List&& l) noexcept;

		template<typename List>
		friend class ListIterator;
	private:		
		struct Node
		{		
			Node* next;
			ValueType value;
			uint iteratorCount;

			template<typename ... Args> requires ::std::constructible_from<T, Args...>
			Node(Node* next, Args&& ... args);
		};

		//Wont free previous contents
		void CopyUnsafe(const List& other);

		Node* first;
		Node* last;
		size_t count;		
		BLAZE_ALLOCATOR_ATTRIBUTE Allocator allocator;
	};
}