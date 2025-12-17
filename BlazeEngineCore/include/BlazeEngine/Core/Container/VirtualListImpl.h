#pragma once

namespace Blaze
{
	template<typename T>
	inline VirtualListNodeBase<T>::VirtualListNodeBase(VirtualListNodeBase* next, T& valueBase)
		: next(next), valueBase(valueBase)
	{
	}
	template<typename T>
	inline VirtualListNodeBase<T>::~VirtualListNodeBase()
	{
	}

	template<typename Base, typename Derived> requires IsDerivedFrom<Derived, Base> || SameAs<Base, void>
	template<typename ... Args> requires IsConstructibleFrom<Derived, Args...>
	inline VirtualListNode<Base, Derived>::VirtualListNode(VirtualListNodeBase<Base>* next, Args && ...args)
		: VirtualListNodeBase<Base>(next, value), value(std::forward<Args>(args)...)
	{
	}
	template<typename Base, typename Derived> requires IsDerivedFrom<Derived, Base> || SameAs<Base, void>
	inline VirtualListNode<Base, Derived>::~VirtualListNode()
	{
	}

	template<typename T, bool IsConst>
	inline VirtualListIterator<T, IsConst> ::VirtualListIterator()
		: nodeBase(nullptr)
	{
	}
	template<typename T, bool IsConst>
	inline VirtualListIterator<T, IsConst>::VirtualListIterator(NodeBase* nodeBase)
		: nodeBase(nodeBase)
	{
	}
	template<typename T, bool IsConst>
	template<bool IsConst2> requires (IsConst || !IsConst2)
	inline VirtualListIterator<T, IsConst>::VirtualListIterator(const VirtualListIterator<T, IsConst2>& other)
		: nodeBase(other.nodeBase)
	{
	}
	template<typename T, bool IsConst>
	inline bool VirtualListIterator<T, IsConst>::IsNull() const
	{
		return nodeBase == nullptr;
	}
	template<typename T, bool IsConst>
	inline VirtualListIterator<T, IsConst>& VirtualListIterator<T, IsConst>::operator++()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (nodeBase == nullptr)
			BLAZE_LOG_FATAL("Incrementing a null iterator");
#endif

		nodeBase = nodeBase->next;
		return *this;
	}
	template<typename T, bool IsConst>
	inline VirtualListIterator<T, IsConst> VirtualListIterator<T, IsConst>::operator++(int)
	{
		VirtualListIterator copy = *this;
		++copy;
		return copy;
	}
	template<typename T, bool IsConst>
	inline auto VirtualListIterator<T, IsConst>::operator*() const -> ValueBaseType& requires (!SameAs<RemoveConst<T>, void>)
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (nodeBase == nullptr)
			BLAZE_LOG_FATAL("Dereferencing a null iterator");
#endif

		return nodeBase->valueBase;
	}

	template<typename T, bool IsConst>
	inline auto VirtualListIterator<T, IsConst>::operator->() const -> ValueBaseType* requires (!SameAs<RemoveConst<T>, void>)
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (nodeBase == nullptr)
			BLAZE_LOG_FATAL("Dereferencing a null iterator");
#endif

		return &nodeBase->valueBase;
	}
	template<typename T, bool IsConst>
	template<typename T2> requires (IsConstType<T2> || !IsConstType<typename VirtualListIterator<T, IsConst>::ValueBaseType>) && (IsDerivedFrom<T2, T> || SameAs<RemoveConst<T>, void>)
	inline Conditional<IsConst, const T2, T2>* VirtualListIterator<T, IsConst>::GetValue() const
	{
		if (nodeBase == nullptr)
			return nullptr;

		return dynamic_cast<T2*>(&nodeBase->valueBase);
	}
	template<typename T, bool IsConst>
	template<bool IsConst2> requires (IsConst || !IsConst2)
	inline bool VirtualListIterator<T, IsConst>::operator==(const VirtualListIterator<T, IsConst2>& other) const
	{
		return nodeBase == other.nodeBase;
	}
	template<typename T, bool IsConst>
	template<bool IsConst2> requires (IsConst || !IsConst2)
	inline bool VirtualListIterator<T, IsConst>::operator!=(const VirtualListIterator<T, IsConst2>& other) const
	{
		return nodeBase != other.nodeBase;
	}
	template<typename T, bool IsConst>
	template<bool IsConst2> requires (IsConst || !IsConst2)
	inline VirtualListIterator<T, IsConst>& VirtualListIterator<T, IsConst>::operator=(const VirtualListIterator<T, IsConst2>& other)
	{
		nodeBase = other.nodeBase;

		return *this;
	}

	template<typename T, AllocatorType Allocator>
	inline VirtualList<T, Allocator>::VirtualList()
		: first(nullptr), last(nullptr), count(0)
	{
	}
	template<typename T, AllocatorType Allocator>
	inline VirtualList<T, Allocator>::VirtualList(VirtualList&& list) noexcept
	{
		first = list.first;
		last = list.last;
		count = list.count;
		list.first = nullptr;
		list.last = nullptr;
		list.count = 0;
	}
	template<typename T, AllocatorType Allocator>
	inline VirtualList<T, Allocator>::~VirtualList()
	{
		Clear();
	}
	template<typename T, AllocatorType Allocator>
	inline void VirtualList<T, Allocator>::Clear()
	{
		if (count == 0)
			return;

		NodeBase* it = first;

		while (it != nullptr)
		{
			NodeBase* next = it->next;

			std::destroy_at(it);
			allocator.Free(it);

			it = next;
		}

		count = 0;
		first = 0;
		last = 0;
	}
	template<typename T, AllocatorType Allocator>
	inline bool VirtualList<T, Allocator>::Empty() const
	{
		return count == 0;
	}
	template<typename T, AllocatorType Allocator>
	inline uintMem VirtualList<T, Allocator>::Count() const
	{
		return count;
	}
	template<typename T, AllocatorType Allocator>
	template<typename Derived, typename ... Args> requires InsertableToVirtualList<Derived, T>&& IsConstructibleFrom<Derived, Args...>
	inline VirtualList<T, Allocator>::Iterator VirtualList<T, Allocator>::AddAfter(const Iterator& it, Args&& ...args)
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (it.IsNull())
			BLAZE_LOG_FATAL("Iterator is null");
#endif

		NodeBase* prev = it.node;
		NodeBase* next = prev->next;
		Node<Derived>* nodeReal = (Node<Derived>*)allocator.Allocate(sizeof(Node<Derived>));
		NodeBase* node = (NodeBase*)nodeReal;
		prev->next = node;
		std::construct_at(nodeReal, next, std::forward<Args>(args)...);

		if (next == nullptr)
			last = node;

		++count;
		return Iterator(node);
	}
	template<typename T, AllocatorType Allocator>
	template<typename Derived, typename ... Args> requires InsertableToVirtualList<Derived, T>&& IsConstructibleFrom<Derived, Args...>
	inline auto VirtualList<T, Allocator>::AddBack(Args&& ... args) -> Iterator
	{
		if (count == 0)
			AddFront<Derived, Args...>(std::forward<Args>(args)...);
		else
		{
			Node<Derived>* node = (Node<Derived>*)allocator.Allocate(sizeof(Node<Derived>));
			last->next = (NodeBase*)node;
			std::construct_at(node, nullptr, std::forward<Args>(args)...);

			last = last->next;
			++count;
		}

		return Iterator(last);
	}
	template<typename T, AllocatorType Allocator>
	template<typename Derived, typename ... Args> requires InsertableToVirtualList<Derived, T>&& IsConstructibleFrom<Derived, Args...>
	inline auto VirtualList<T, Allocator>::AddFront(Args&& ... args) -> Iterator
	{
		NodeBase* second = first;

		Node<Derived>* node = (Node<Derived>*)allocator.Allocate(sizeof(Node<Derived>));
		first = (NodeBase*)node;
		std::construct_at(node, second, std::forward<Args>(args)...);

		++count;

		if (count == 1)
			last = first;

		return Iterator(first);
	}
	template<typename T, AllocatorType Allocator>
	inline void VirtualList<T, Allocator>::EraseAfter(const Iterator& it)
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
		{
			BLAZE_LOG_ERROR("Erasing from an empty list");
			return;
		}

		if (it.IsNull())
		{
			BLAZE_LOG_ERROR("Iterator is null");
			return;
		}
#endif

		NodeBase* prev = it.nodeBase;
		NodeBase* node = prev->next;

#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (node == nullptr)
		{
			BLAZE_LOG_ERROR("Iterator is the last in the list");
			return;
		}
#endif

		NodeBase* next = node->next;

		std::destroy_at(node);
		allocator.Free(node);

		prev->next = next;

		if (next == nullptr)
			last = prev;
		--count;

		if (count == 0)
		{
			first = nullptr;
			last = nullptr;
		}
	}
	template<typename T, AllocatorType Allocator>
	inline void VirtualList<T, Allocator>::EraseFirst()
	{
		if (count == 0)
		{
			BLAZE_LOG_ERROR("Erasing from an empty list");
			return;
		}

		NodeBase* next = first->next;
		NodeBase* node = first;

		std::destroy_at(node);
		allocator.Free(node);

		first = next;
		--count;

		if (count == 0)
		{
			first = nullptr;
			last = nullptr;
		}
		else if (count == 1)
		{
			last = first;
		}
	}
	template<typename T, AllocatorType Allocator>
	template<typename C> requires std::invocable<C, T&>&& SameAs<std::invoke_result_t<C, T&>, bool>
	inline void VirtualList<T, Allocator>::EraseAll(const C& function)
	{
		if (count == 0)
			return;

		NodeBase* prev = first;
		NodeBase* curr = prev->next;

		if (function(first->valueBase))
			EraseFirst();

		while (curr != nullptr)
		{
			NodeBase* next = curr->next;

			if (function(curr->valueBase))
			{
				EraseAfter(Iterator(prev));

				curr = next;
			}
			else
			{
				prev = curr;
				curr = next;
			}
		}
	}
	template<typename T, AllocatorType Allocator>
	template<typename C> requires std::invocable<C, T&>&& SameAs<std::invoke_result_t<C, T&>, bool>
	inline void VirtualList<T, Allocator>::EraseOne(const C& function)
	{
		if (count == 0)
		{
			BLAZE_LOG_ERROR("Erasing from an empty list");
			return;
		}

		NodeBase* prev = first;
		NodeBase* curr = prev->next;

		if (function(first->valueBase))
		{
			EraseFirst();
			return;
		}

		while (curr != nullptr)
		{
			if (function(curr->valueBase))
			{
				EraseAfter(Iterator(prev));
				return;
			}

			prev = curr;
			curr = curr->next;
		}
	}
	template<typename T, AllocatorType Allocator>
	inline void VirtualList<T, Allocator>::AppendBack(const VirtualList& list)
	{
		for (const auto& el : list)
			AddBack(el);
	}
	template<typename T, AllocatorType Allocator>
	inline void VirtualList<T, Allocator>::AppendBack(VirtualList&& list)
	{
		if (list.count == 0)
		{
			first = list.first;
			last = list.last;
			count = list.count;
		}
		else
		{
			this->last->next = list.first;
			this->last = list.last;
			this->count += list.count;
		}

		list.first = nullptr;
		list.last = nullptr;
		list.count = 0;
	}
	template<typename T, AllocatorType Allocator>
	inline void Blaze::VirtualList<T, Allocator>::AppendFront(const VirtualList& list)
	{
		for (const auto& el : list)
			AddFront(el);
	}
	template<typename T, AllocatorType Allocator>
	inline void Blaze::VirtualList<T, Allocator>::AppendFront(VirtualList&& list)
	{
		if (list.count == 0)
		{
			first = list.first;
			last = list.last;
			count = list.count;
		}
		else
		{
			list.last->next = this->first;
			this->first = list.first;
			this->count += list.count;
		}

		list.first = nullptr;
		list.last = nullptr;
		list.count = 0;
	}
	template<typename T, AllocatorType Allocator>
	inline VirtualList<T, Allocator>::Iterator VirtualList<T, Allocator>::FirstIterator()
	{
		return Iterator(first);
	}
	template<typename T, AllocatorType Allocator>
	inline VirtualList<T, Allocator>::ConstIterator VirtualList<T, Allocator>::FirstIterator() const
	{
		return ConstIterator(first);
	}
	template<typename T, AllocatorType Allocator>
	inline VirtualList<T, Allocator>::Iterator VirtualList<T, Allocator>::LastIterator()
	{
		return Iterator(last);
	}
	template<typename T, AllocatorType Allocator>
	inline VirtualList<T, Allocator>::ConstIterator VirtualList<T, Allocator>::LastIterator() const
	{
		return ConstIterator(last);
	}
	template<typename T, AllocatorType Allocator>
	inline VirtualList<T, Allocator>::Iterator VirtualList<T, Allocator>::AheadIterator()
	{
		return Iterator();
	}
	template<typename T, AllocatorType Allocator>
	inline VirtualList<T, Allocator>::ConstIterator VirtualList<T, Allocator>::AheadIterator() const
	{
		return ConstIterator();
	}
	template<typename T, AllocatorType Allocator>
	inline VirtualList<T, Allocator>::Iterator VirtualList<T, Allocator>::BehindIterator()
	{
		return Iterator(nullptr);
	}
	template<typename T, AllocatorType Allocator>
	inline VirtualList<T, Allocator>::ConstIterator VirtualList<T, Allocator>::BehindIterator() const
	{
		return ConstIterator(nullptr);
	}
	template<typename T, AllocatorType Allocator>
	inline VirtualList<T, Allocator>& VirtualList<T, Allocator>::operator=(VirtualList&& other) noexcept
	{
		Clear();
		first = other.first;
		last = other.last;
		count = other.count;
		other.first = nullptr;
		other.last = nullptr;
		other.count = 0;
		return *this;
	}
}