#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/Memory/Allocator.h"
#include <type_traits>

namespace Blaze
{
	template<typename T, AllocatorType Allocator>
	class VirtualList;

	template<typename T>
	struct VirtualListNodeBase
	{
		VirtualListNodeBase* next;
		T& valueBase;

		VirtualListNodeBase(VirtualListNodeBase* next, T& valueBase);
		virtual ~VirtualListNodeBase();
	};
	template<typename Base, typename Derived> requires IsDerivedFrom<Derived, Base> || SameAs<Base, void>
	struct VirtualListNode : public VirtualListNodeBase<Base>
	{
		Derived value;

		template<typename ... Args> requires IsConstructibleFrom<Derived, Args...>
		VirtualListNode(VirtualListNodeBase<Base>* next, Args&& ... args);
		~VirtualListNode() override;
	};

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
	template<typename T, bool IsConst = false>
	class BLAZE_API VirtualListIterator
	{
	public:
		using NodeBase = Conditional<IsConst, const VirtualListNodeBase<T>, VirtualListNodeBase<T>>;
		using ValueBaseType = Conditional<IsConst, const T, T>;

		VirtualListIterator();
		VirtualListIterator(NodeBase* nodeBase);
		template<bool IsConst2> requires (IsConst || !IsConst2)
		VirtualListIterator(const VirtualListIterator<T, IsConst2>& other);

		bool IsNull() const;

		VirtualListIterator& operator++();
		VirtualListIterator operator++(int);

		//Returns the value that the iterator is pointing to. If the asked type does not match
		//the stored value nullptr is returned. If the iterator is null nullptr is returned
		template<typename T2> requires (IsConstType<T2> || !IsConstType<typename VirtualListIterator<T, IsConst>::ValueBaseType>) && (IsDerivedFrom<T2, T> || SameAs<RemoveConst<T>, void>)
		Conditional<IsConst, const T2, T2>* GetValue() const;

		ValueBaseType& operator*() const requires (!SameAs<RemoveConst<T>, void>);
		ValueBaseType* operator->() const requires (!SameAs<RemoveConst<T>, void>);

		template<bool IsConst2> requires (IsConst || !IsConst2)
		bool operator==(const VirtualListIterator<T, IsConst2>& other) const;
		template<bool IsConst2> requires (IsConst || !IsConst2)
		bool operator!=(const VirtualListIterator<T, IsConst2>& other) const;

		template<bool IsConst2> requires (IsConst || !IsConst2)
		VirtualListIterator& operator=(const VirtualListIterator<T, IsConst2>& other);

		template<typename, AllocatorType>
		friend class ::Blaze::VirtualList;
	private:
		NodeBase* nodeBase;
	};

	template<typename Value, typename ValueBase>
	concept InsertableToVirtualList = IsDerivedFrom<Value, ValueBase> || SameAs<ValueBase, void>;

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
	class BLAZE_API VirtualList
	{
	public:
		using Iterator = VirtualListIterator<T, false>;
		using ConstIterator = VirtualListIterator<T, true>;
		using ValueBaseType = T;

		VirtualList();
		VirtualList(const VirtualList& list) = delete;
		VirtualList(VirtualList&& list) noexcept;
		~VirtualList();

		void Clear();
		bool Empty() const;
		uintMem Count() const;

		template<typename Derived, typename ... Args> requires InsertableToVirtualList<Derived, T> && IsConstructibleFrom<Derived, Args...>
		Iterator AddAfter(const Iterator& it, Args&& ... args);
		template<typename Derived, typename ... Args> requires InsertableToVirtualList<Derived, T> && IsConstructibleFrom<Derived, Args...>
		Iterator AddBack(Args&& ... args);
		template<typename Derived, typename ... Args> requires InsertableToVirtualList<Derived, T> && IsConstructibleFrom<Derived, Args...>
		Iterator AddFront(Args&& ... args);

		void EraseAfter(const Iterator& it);
		void EraseFirst();
		template<typename C> requires std::invocable<C, T&>&& SameAs<std::invoke_result_t<C, T&>, bool>
		void EraseAll(const C& function);
		template<typename C> requires std::invocable<C, T&>&& SameAs<std::invoke_result_t<C, T&>, bool>
		void EraseOne(const C& function);

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
	private:
		using NodeBase = VirtualListNodeBase<T>;
		template<typename Derived>
		using Node = VirtualListNode<T, Derived>;
		//Wont free previous contents
		void CopyUnsafe(const VirtualList& other);

		VirtualListNodeBase<T>* first;
		VirtualListNodeBase<T>* last;
		size_t count;
		BLAZE_ALLOCATOR_ATTRIBUTE Allocator allocator;
	};

	template<typename T, AllocatorType Allocator>
	VirtualList<T, Allocator>::Iterator begin(VirtualList<T, Allocator>& map)
	{
		return map.FirstIterator();
	}
	template<typename T, AllocatorType Allocator>
	VirtualList<T, Allocator>::ConstIterator begin(const VirtualList<T, Allocator>& map)
	{
		return map.FirstIterator();
	}
	template<typename T, AllocatorType Allocator>
	VirtualList<T, Allocator>::Iterator end(VirtualList<T, Allocator>& map)
	{
		return map.BehindIterator();
	}
	template<typename T, AllocatorType Allocator>
	VirtualList<T, Allocator>::ConstIterator end(const VirtualList<T, Allocator>& map)
	{
		return map.BehindIterator();
	}
}

#include "BlazeEngine/Core/Container/VirtualListImpl.h"