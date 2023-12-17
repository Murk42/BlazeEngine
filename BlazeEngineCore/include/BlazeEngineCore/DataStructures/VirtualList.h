#pragma once

namespace Blaze
{
	template<typename T, AllocatorType Allocator>
	class VirtualList;

	template<typename T1, typename T2>
	concept IsConvertibleToVirtualListIterator =
		std::same_as<std::remove_const_t<typename T2::template VirtualListType>, std::remove_const_t<typename T1::template VirtualListType>> &&
		(!(std::is_const_v<typename T1::template VirtualListType> && !std::is_const_v<typename T2::template VirtualListType>));

	template<typename T1, typename T2>
	concept IsComparableToVirtualListIterator =
		std::same_as<std::remove_const_t<typename T2::template VirtualListType>, std::remove_const_t<typename T1::template VirtualListType>>;

	/*
		Used with the Blaze::VirtualList class.

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
	template<typename VirtualList>
	class BLAZE_CORE_API VirtualListIterator
	{
	public:		
		template<typename T>
		using MatchValueBaseConst = std::conditional_t<std::is_const_v<typename VirtualList::template ValueBaseType> || std::is_const_v<VirtualList>, const std::remove_const_t<T>, std::remove_const_t<T>>;
		using VirtualListType = VirtualList;
		using ValueBaseType = MatchValueBaseConst<typename VirtualList::template ValueBaseType>;

		VirtualListIterator();
		template<IsConvertibleToVirtualListIterator<VirtualListIterator<VirtualList>> T>
		VirtualListIterator(const T&);

		bool IsNull() const;

		VirtualListIterator& operator++();
		VirtualListIterator operator++(int);

		template<typename Value> requires InsertableToVirtualList<Value, typename VirtualList::template ValueBaseType>
		MatchValueBaseConst<Value>* GetValue() const;		

		ValueBaseType& operator*() const requires (!std::same_as<ValueBaseType, void>);
		ValueBaseType* operator->() const requires (!std::same_as<ValueBaseType, void>);

		template<IsComparableToVirtualListIterator<VirtualListIterator<VirtualList>> T>
		bool operator==(const T& i) const;
		template<IsComparableToVirtualListIterator<VirtualListIterator<VirtualList>> T>
		bool operator!=(const T& i) const;

		template<IsConvertibleToVirtualListIterator<VirtualListIterator<VirtualList>> T>
		VirtualListIterator& operator=(const T& i);

		template<typename, AllocatorType>
		friend class ::Blaze::VirtualList;
	private:
		using Node = ::std::conditional_t<std::is_const_v<VirtualList>, const typename VirtualList::template NodeHeader, typename VirtualList::template NodeHeader>;

		Node* node;

		VirtualListIterator(Node* node);
	};

	template<typename Value, typename ValueBase>
	concept InsertableToVirtualList = std::derived_from<Value, ValueBase> || std::same_as<ValueBase, void>;

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
	class BLAZE_CORE_API VirtualList
	{
	public:
		using Iterator = VirtualListIterator<VirtualList>;
		using ConstIterator = VirtualListIterator<const VirtualList>;
		using ValueBaseType = T;				

		VirtualList();
		VirtualList(const VirtualList& list) = delete;
		VirtualList(VirtualList&& list) noexcept;
		~VirtualList();

		void Clear();
		bool Empty() const;
		uintMem Count() const;

		template<typename Derived, typename ... Args> requires InsertableToVirtualList<Derived, T> && std::constructible_from<Derived, Args...>
		Iterator AddAfter(const Iterator& it, Args&& ... args);
		template<typename Derived, typename ... Args> requires InsertableToVirtualList<Derived, T> && std::constructible_from<Derived, Args...>
		Iterator AddBack(Args&& ... args);										 
		template<typename Derived, typename ... Args> requires InsertableToVirtualList<Derived, T> && std::constructible_from<Derived, Args...>
		Iterator AddFront(Args&& ... args);

		Result EraseAfter(const Iterator& it);
		Result EraseFirst();
		template<typename C> requires std::invocable<C, T&>&& std::same_as<std::invoke_result_t<C, T&>, bool>
		Result EraseAll(const C& function);
		template<typename C> requires std::invocable<C, T&>&& std::same_as<std::invoke_result_t<C, T&>, bool>
		Result EraseOne(const C& function);

		void AppendBack(const VirtualList& list);
		void AppendBack(VirtualList&& list);
		void AppendFront(const VirtualList& list);
		void AppendFront(VirtualList&& list);		

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

		VirtualList& operator=(const VirtualList& l) = delete;
		VirtualList& operator=(VirtualList&& l) noexcept;

		template<typename VirtualList>
		friend class VirtualListIterator;
	private:
		struct NodeHeader
		{
			NodeHeader* next;			
			T* valueBase;
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
			uintMem iteratorCount;
#endif
			
			NodeHeader(NodeHeader* next, T* valueBase);
			virtual ~NodeHeader();
		};
		template<typename Value> requires InsertableToVirtualList<Value, T>
		struct Node : public NodeHeader
		{
			Value value;

			template<typename ... Args> requires std::constructible_from<Value, Args...>
			Node(NodeHeader* next, Args&& ... args);
			~Node() override;
		};

		//Wont free previous contents
		void CopyUnsafe(const VirtualList& other);

		NodeHeader* first;
		NodeHeader* last;
		size_t count;
		BLAZE_ALLOCATOR_ATTRIBUTE Allocator allocator;
	};	
}