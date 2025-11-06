#pragma once
#include <memory>

namespace Blaze
{
	template<typename List>
	inline ListIterator<List>::ListIterator()
		: node(nullptr)
	{
	}
	template<typename List>
	template<IsConvertibleToListIterator<ListIterator<List>> T>
	inline ListIterator<List>::ListIterator(const T& i)
		: node(i.node)
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (node != nullptr)
			++node->iteratorCount;
#endif
	}
	template<typename List>
	inline bool ListIterator<List>::IsNull() const
	{
		return node == nullptr;
	}
	template<typename List>
	inline auto ListIterator<List>::Ptr() const -> ValueType*
	{
		if (node == nullptr)
			return nullptr;

		return &node->value;
	}
	template<typename List>
	inline ListIterator<List>& ListIterator<List>::operator++()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (node == nullptr)
			BLAZE_LOG_FATAL("Incrementing a null iterator");
#endif

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		--node->iteartorCount;
#endif

		node = node->next;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (node != nullptr)
			++node->iteartorCount;
#endif
		return *this;
	}
	template<typename List>
	inline ListIterator<List> ListIterator<List>::operator++(int)
	{
		ListIterator copy = *this;
		++copy;
		return copy;
	}
	template<typename List>
	inline ListIterator<List>::ValueType& ListIterator<List>::operator*() const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (node == nullptr)
			BLAZE_LOG_FATAL("Dereferencing a null iterator");
#endif

		return node->value;
	}
	template<typename List>
	inline ListIterator<List>::ValueType* ListIterator<List>::operator->() const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (node == nullptr)
			BLAZE_LOG_FATAL("Dereferencing a null iterator");
#endif

		return &node->value;
	}
	template<typename List>
	template<IsComparableToListIterator<ListIterator<List>> T>
	inline bool ListIterator<List>::operator==(const T& i) const
	{
		return node == i.node;
	}
	template<typename List>
	template<IsComparableToListIterator<ListIterator<List>> T>
	inline bool ListIterator<List>::operator!=(const T& i) const
	{
		return node != i.node;
	}
	template<typename List>
	template<IsConvertibleToListIterator<ListIterator<List>> T>
	inline ListIterator<List>& ListIterator<List>::operator=(const T& i)
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (node != nullptr)
			--node.iteratorCount;
#endif
		node = i.node;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		++node->iteartorCount;
#endif
		return *this;
	}
	template<typename List>
	inline ListIterator<List>::ListIterator(Node* node)
		: node(node)
	{
	}

	template<typename T, AllocatorType Allocator>
	inline List<T, Allocator>::List()
		: first(nullptr)
	{
	}
	template<typename T, AllocatorType Allocator>
	inline List<T, Allocator>::List(const List& other) requires std::is_copy_constructible_v<T>
		: first(nullptr)
	{
		CopyUnsafe(other);
	}
	template<typename T, AllocatorType Allocator>
	inline List<T, Allocator>::List(List&& list) noexcept
	{
		first = list.first;
		list.first = nullptr;
	}
	template<typename T, AllocatorType Allocator>
	inline List<T, Allocator>::~List()
	{
		Clear();
	}
	template<typename T, AllocatorType Allocator>
	inline void List<T, Allocator>::Clear()
	{
		if (first == nullptr)
			return;

		Node* it = first;

		while (it != nullptr)
		{
			Node* next = it->next;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
			if (it->iteratorCount != 0)
				BLAZE_LOG_WARNING("Clearing a list while a iterator is referencing it");
#endif

			std::destroy_at(it);
			allocator.Free(it);

			it = next;
		}

		first = 0;
	}
	template<typename T, AllocatorType Allocator>
	inline bool List<T, Allocator>::Empty() const
	{
		return first == nullptr;
	}
	template<typename T, AllocatorType Allocator>
	template<typename ...Args> requires IsConstructibleFrom<T, Args...>
	inline auto List<T, Allocator>::AddAfter(const Iterator& it, Args&& ...args) -> Iterator
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (it.IsNull())
			BLAZE_LOG_FATAL("Iterator is null");
#endif

		Node* prev = it.node;
		Node* next = prev->next;
		Node* node = (Node*)allocator.Allocate(sizeof(Node));
		prev->next = node;
		std::construct_at(node, next, std::forward<Args>(args)...);

		return Iterator(node);
	}
	template<typename T, AllocatorType Allocator>
	template<typename ... Args> requires IsConstructibleFrom<T, Args...>
	inline auto List<T, Allocator>::AddFront(Args&& ... args) -> Iterator
	{
		Node* second = first;

		first = (Node*)allocator.Allocate(sizeof(Node));
		std::construct_at(first, second, std::forward<Args>(args)...);

		return Iterator(first);
	}
	template<typename T, AllocatorType Allocator>
	inline void List<T, Allocator>::EraseAfter(const Iterator& it)
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (first == nullptr)
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

		Node* prev = it.node;
		Node* node = prev->next;

#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (node == nullptr)
		{
			BLAZE_LOG_ERROR("Iterator is the last in the list");
			return;
		}
#endif

		Node* next = node->next;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (node->iteratorCount != 0)
			BLAZE_LOG_WARNING("Erasing a list element while a iterator is referencing it");
#endif

		std::destroy_at(node);
		allocator.Free(node);

		prev->next = next;
	}
	template<typename T, AllocatorType Allocator>
	inline void List<T, Allocator>::EraseFirst()
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (first == nullptr)
		{
			BLAZE_LOG_ERROR("Erasing from an empty list");
			return;
		}
#endif
		Node* next = first->next;
		Node* node = first;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (node->iteratorCount != 0)
			BLAZE_LOG_WARNING("Erasing a list element while a iterator is referencing it");
#endif

		std::destroy_at(node);
		allocator.Free(node);

		first = next;
	}
	template<typename T, AllocatorType Allocator>
	template<typename C> requires std::invocable<C, T&>&& SameAs<std::invoke_result_t<C, T&>, bool>
	inline void List<T, Allocator>::EraseAll(const C& function)
	{
		if (first == nullptr)
			return;

		while (function(first->value))
		{
			EraseFirst();

			if (first == nullptr)
				return;
		}

		Node* prev = first;
		Node* curr = first->next;

		while (curr != nullptr)
		{
			Node* next = curr->next;

			if (function(curr->value))
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
	inline void List<T, Allocator>::EraseOne(const C& function)
	{
		if (first == nullptr)
		{
			BLAZE_LOG_FATAL("Erasing from an empty list");
			return;
		}

		if (function(first->value))
		{
			EraseFirst();
			return;
		}

		Node* prev = first;
		Node* curr = first->next;

		while (curr != nullptr)
		{
			if (function(curr->value))
			{
				EraseAfter(Iterator(prev));
				return;
			}

			prev = curr;
			curr = curr->next;
		}
	}
	template<typename T, AllocatorType Allocator>
	inline List<T, Allocator> List<T, Allocator>::SplitAfter(const Iterator& it)
	{
		List<T, Allocator> out;
		out.first = it.node->next;

		it.node->next = nullptr;
		return out;
	}
	template<typename T, AllocatorType Allocator>
	inline T& List<T, Allocator>::First()
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (first == nullptr)
			BLAZE_LOG_FATAL("List is empty");
#endif
		return first->value;
	}
	template<typename T, AllocatorType Allocator>
	inline const T& List<T, Allocator>::First() const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (first == nullptr)
			BLAZE_LOG_FATAL("List is empty");
#endif
		return first->value;
	}
	template<typename T, AllocatorType Allocator>
	inline List<T, Allocator>::Iterator List<T, Allocator>::FirstIterator()
	{
		return Iterator(first);
	}
	template<typename T, AllocatorType Allocator>
	inline List<T, Allocator>::ConstIterator List<T, Allocator>::FirstIterator() const
	{
		return ConstIterator(first);
	}
	template<typename T, AllocatorType Allocator>
	inline List<T, Allocator>::Iterator List<T, Allocator>::BehindIterator()
	{
		return Iterator(nullptr);
	}
	template<typename T, AllocatorType Allocator>
	inline List<T, Allocator>::ConstIterator List<T, Allocator>::BehindIterator() const
	{
		return ConstIterator(nullptr);
	}
	template<typename T, AllocatorType Allocator>
	inline List<T, Allocator>& List<T, Allocator>::operator=(const List& other) requires std::is_copy_assignable_v<T>
	{
		Clear();

		CopyUnsafe(other);

		return *this;
	}
	template<typename T, AllocatorType Allocator>
	inline List<T, Allocator>& List<T, Allocator>::operator=(List&& other) noexcept
	{
		Clear();
		first = other.first;
		other.first = nullptr;
		return *this;
	}
	template<typename T, AllocatorType Allocator>
	template<typename ... Args> requires IsConstructibleFrom<T, Args...>
	inline List<T, Allocator>::Node::Node(Node* next, Args&& ... args)
		: value(std::forward<Args>(args)...), next(next)
	{
	}
	template<typename T, AllocatorType Allocator>
	inline void List<T, Allocator>::CopyUnsafe(const List& other)
	{
		if (other.first == nullptr)
			return;

		first = (Node*)allocator.Allocate(sizeof(Node));
		std::construct_at(first, nullptr, (const T&)*other.first);

		Node* itDst = first;
		Node* itSrc = other.first;

		while (itSrc->next != nullptr)
		{
			itSrc = itSrc->next;

			itDst->next = (Node*)allocator.Allocate(sizeof(Node));
			itDst = itDst->next;

			std::construct_at(itDst, nullptr, (const T&)*itSrc);
		}
	}
}