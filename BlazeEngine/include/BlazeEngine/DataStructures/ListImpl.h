#pragma once
#include "List.h"

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
	inline ListIterator<List>& ListIterator<List>::operator++()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (node == nullptr)
			Debug::Logger::LogFatal("Blaze Engine", "Incrementing a null iterator");
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
			Debug::Logger::LogFatal("Blaze Engine", "Dereferencing a null iterator");
#endif

		return node->value;
	}
	template<typename List>
	inline ListIterator<List>::ValueType* ListIterator<List>::operator->() const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (node == nullptr)
			Debug::Logger::LogFatal("Blaze Engine", "Dereferencing a null iterator");
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
		: first(nullptr), last(nullptr), count(0)
	{
	}	
	template<typename T, AllocatorType Allocator>	
	inline List<T, Allocator>::List(const List& other) requires std::is_copy_constructible_v<T>
		: first(nullptr), last(nullptr), count(0)
	{	
		CopyUnsafe(other);
	}
	template<typename T, AllocatorType Allocator>
	inline List<T, Allocator>::List(List&& list) noexcept
	{
		first = list.first;
		last = list.last;
		count = list.count;
		list.first = nullptr;
		list.last = nullptr;
		list.count = 0;
	}
	template<typename T, AllocatorType Allocator>
	inline List<T, Allocator>::~List()
	{
		Clear();
	}
	template<typename T, AllocatorType Allocator>
	inline void List<T, Allocator>::Clear()
	{
		if (count == 0)
			return;

		Node* it = first;

		while (it != nullptr)
		{
			Node* next = it->next;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
			if (it->iteratorCount != 0)
				Debug::Logger::LogWarning("Blaze Engine", "Clearing a list while a iterator is referencing it");
#endif

			std::destroy_at(it);
			allocator.Free(it);

			it = next;
		}

		count = 0;
		first = 0;
		last = 0;
	}
	template<typename T, AllocatorType Allocator>
	inline bool List<T, Allocator>::Empty() const
	{
		return count == 0;
	}
	template<typename T, AllocatorType Allocator>
	inline uintMem List<T, Allocator>::Count() const
	{
		return count;
	}
	template<typename T, AllocatorType Allocator>
	template<typename ...Args> requires std::constructible_from<T, Args...>
	inline List<T, Allocator>::Iterator List<T, Allocator>::AddAfter(const Iterator& it, Args&& ...args)
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (it.IsNull())
			Debug::Logger::LogFatal("Blaze Engine", "Iterator is null");
#endif		

		Node* prev = it.node;		
		Node* next = prev->next;		
		Node* node = (Node*)allocator.Allocate(sizeof(Node));
		prev->next = node;
		std::construct_at(node, next, std::forward<Args>(args)...);

		if (next == nullptr)
			last = node;

		++count;
		return Iterator(node);
	}
	template<typename T, AllocatorType Allocator>
	template<typename ... Args> requires std::constructible_from<T, Args...>
	inline List<T, Allocator>::template Iterator List<T, Allocator>::AddBack(Args&& ... args)
	{
		if (count == 0)
			AddFront(std::forward<Args>(args)...);
		else
		{
			last->next = (Node*)allocator.Allocate(sizeof(Node));
			std::construct_at(last->next, nullptr, std::forward<Args>(args)...);

			last = last->next;
			++count;
		}

		return Iterator(last);
	}
	template<typename T, AllocatorType Allocator>
	template<typename ... Args> requires std::constructible_from<T, Args...>
	inline List<T, Allocator>::template Iterator List<T, Allocator>::AddFront(Args&& ... args)
	{
		Node* second = first;

		first = (Node*)allocator.Allocate(sizeof(Node));
		std::construct_at(first, second, std::forward<Args>(args)...);

		++count;

		if (count == 1)
			last = first;

		return Iterator(first);
	}
	template<typename T, AllocatorType Allocator>
	inline Result List<T, Allocator>::EraseAfter(const Iterator& it)
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			return BLAZE_ERROR_RESULT("Blaze Engine", "Erasing from an empty list");

		if (it.IsNull())
			return BLAZE_ERROR_RESULT("Blaze Engine", "Iterator is null");
#endif

		Node* prev = it.node;
		Node* node = prev->next;

#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (node == nullptr)
			return BLAZE_ERROR_RESULT("Blaze Engine", "Iterator is the last in the list");
#endif

		Node* next = node->next;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (node->iteratorCount != 0)
			Debug::Logger::LogWarning("Blaze Engine", "Erasing a list element while a iterator is referencing it");
#endif

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

		return Result();
	}
	template<typename T, AllocatorType Allocator>
	inline Result List<T, Allocator>::EraseFirst()
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			return BLAZE_ERROR_RESULT("Blaze Engine", "Erasing from an empty list");
#endif
		Node* next = first->next;
		Node* node = first;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (node->iteratorCount != 0)
			Debug::Logger::LogWarning("Blaze Engine", "Erasing a list element while a iterator is referencing it");
#endif

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

		return Result();
	}
	template<typename T, AllocatorType Allocator>
	template<typename C> requires std::invocable<C, T&>&& std::same_as<std::invoke_result_t<C, T&>, bool>
	inline Result List<T, Allocator>::EraseAll(const C& function)
	{
		if (count == 0)
			return Result();

		Node* prev = first;
		Node* curr = prev->next;

		if (function(first->value))
			CHECK_RESULT(EraseFirst())

		while (curr != nullptr)
		{
			if (function(curr->value))
				CHECK_RESULT(EraseAfter(Iterator(curr)));

			prev = curr;
			curr = curr->next;
		}

		return Result();
	}
	template<typename T, AllocatorType Allocator>
	template<typename C> requires std::invocable<C, T&>&& std::same_as<std::invoke_result_t<C, T&>, bool>
	inline Result List<T, Allocator>::EraseOne(const C& function)
	{
		if (count == 0)
			return Result();

		Node* prev = first;
		Node* curr = prev->next;

		if (function(first->value))
		{
			CHECK_RESULT(EraseFirst())
			return Result();
		}

		while (curr != nullptr)
		{
			if (function(curr->value))
			{
				CHECK_RESULT(EraseAfter(Iterator(curr)));
				return Result();
			}

			prev = curr;
			curr = curr->next;
		}

		return Result();
	}
	template<typename T, AllocatorType Allocator>
	inline void List<T, Allocator>::AppendBack(const List& list)
	{
		for (const auto& el : list)
			AddBack(el);
	}
	template<typename T, AllocatorType Allocator>
	inline void List<T, Allocator>::AppendBack(List&& list)
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
	inline void Blaze::List<T, Allocator>::AppendFront(const List& list)
	{
		for (const auto& el : list)
			AddFront(el);
	}
	template<typename T, AllocatorType Allocator>
	inline void Blaze::List<T, Allocator>::AppendFront(List&& list)
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
	inline T& List<T, Allocator>::First()
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			Debug::Logger::LogFatal("Blaze Engine", "List is empty");
#endif
		return first->value;
	}
	template<typename T, AllocatorType Allocator>
	inline const T& List<T, Allocator>::First() const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			Debug::Logger::LogFatal("Blaze Engine", "List is empty");
#endif
		return first->value;
	}
	template<typename T, AllocatorType Allocator>
	inline T& List<T, Allocator>::Last()
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			Debug::Logger::LogFatal("Blaze Engine", "List is empty");
#endif
		return last->value;
	}
	template<typename T, AllocatorType Allocator>
	inline const T& List<T, Allocator>::Last() const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			Debug::Logger::LogFatal("Blaze Engine", "List is empty");
#endif
		return last->value;
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
	inline List<T, Allocator>::Iterator List<T, Allocator>::LastIterator()
	{
		return Iterator(last);
	}
	template<typename T, AllocatorType Allocator>
	inline List<T, Allocator>::ConstIterator List<T, Allocator>::LastIterator() const
	{
		return ConstIterator(last);
	}
	template<typename T, AllocatorType Allocator>
	inline List<T, Allocator>::Iterator List<T, Allocator>::AheadIterator()
	{
		return Iterator();
	}
	template<typename T, AllocatorType Allocator>
	inline List<T, Allocator>::ConstIterator List<T, Allocator>::AheadIterator() const
	{
		return ConstIterator();
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
		last = other.last;
		count = other.count;
		other.first = nullptr;
		other.last = nullptr;
		other.count = 0;
		return *this;
	}
	template<typename T, AllocatorType Allocator>
	template<typename ... Args> requires std::constructible_from<T, Args...>
	inline List<T, Allocator>::Node::Node(Node* next, Args&& ... args)
		: value(std::forward<Args>(args)...), next(next)
	{
	}
	template<typename T, AllocatorType Allocator>
	inline void List<T, Allocator>::CopyUnsafe(const List& other)
	{
		if (other.count == 0)
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

		last = itDst;
		count = other.count;
	}

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