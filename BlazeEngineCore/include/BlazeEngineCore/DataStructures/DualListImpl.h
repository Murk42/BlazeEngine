#pragma once
#include "DualList.h"

namespace Blaze
{
	template<typename DualList>
	inline DualListIterator<DualList>::DualListIterator()
		: node(nullptr)
	{
	}
	template<typename DualList>
	template<IsConvertibleToDualListIterator<DualListIterator<DualList>> T>
	inline DualListIterator<DualList>::DualListIterator(const T& i)
		: node(i.node)
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (node != nullptr)
			++node->iteratorCount;
#endif
	}
	template<typename DualList>
	inline bool DualListIterator<DualList>::IsNull() const
	{
		return node == nullptr;
	}
	template<typename DualList>
	inline DualListIterator<DualList>& DualListIterator<DualList>::operator++()
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
	template<typename DualList>
	inline DualListIterator<DualList> DualListIterator<DualList>::operator++(int)
	{
		DualListIterator copy = *this;
		++copy;
		return copy;
	}
	template<typename DualList>
	inline DualListIterator<DualList>& DualListIterator<DualList>::operator--()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (node == nullptr)
			Debug::Logger::LogFatal("Blaze Engine", "Incrementing a null iterator");
#endif

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		--node->iteartorCount;
#endif

		node = node->prev;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (node != nullptr)
			++node->iteartorCount;
#endif
		return *this;
	}
	template<typename DualList>
	inline DualListIterator<DualList> DualListIterator<DualList>::operator--(int)
	{
		DualListIterator copy = *this;
		--copy;
		return copy;
	}
	template<typename DualList>
	inline DualListIterator<DualList>::ValueType& DualListIterator<DualList>::operator*() const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (node == nullptr)
			Debug::Logger::LogFatal("Blaze Engine", "Dereferencing a null iterator");
#endif

		return node->value;
	}
	template<typename DualList>
	inline DualListIterator<DualList>::ValueType* DualListIterator<DualList>::operator->() const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (node == nullptr)
			Debug::Logger::LogFatal("Blaze Engine", "Dereferencing a null iterator");
#endif

		return &node->value;
	}
	template<typename DualList>
	template<IsComparableToDualListIterator<DualListIterator<DualList>> T>
	inline bool DualListIterator<DualList>::operator==(const T& i) const
	{
		return node == i.node;
	}
	template<typename DualList>
	template<IsComparableToDualListIterator<DualListIterator<DualList>> T>
	inline bool DualListIterator<DualList>::operator!=(const T& i) const
	{
		return node != i.node;
	}
	template<typename DualList>
	template<IsConvertibleToDualListIterator<DualListIterator<DualList>> T>
	inline DualListIterator<DualList>& DualListIterator<DualList>::operator=(const T& i)
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
	template<typename DualList>
	inline DualListIterator<DualList>::DualListIterator(Node* node)
		: node(node)
	{
	}

	template<typename T, AllocatorType Allocator>
	inline DualList<T, Allocator>::DualList()
		: first(nullptr), last(nullptr), count(0)
	{
	}
	template<typename T, AllocatorType Allocator>
	inline DualList<T, Allocator>::DualList(const DualList& other)
		: first(nullptr), last(nullptr), count(0)
	{
		CopyUnsafe(other);
	}
	template<typename T, AllocatorType Allocator>
	inline DualList<T, Allocator>::DualList(DualList&& list) noexcept
	{
		first = list.first;
		last = list.last;
		count = list.count;
		list.first = nullptr;
		list.last = nullptr;
		list.count = 0;
	}
	template<typename T, AllocatorType Allocator>
	inline DualList<T, Allocator>::~DualList()
	{
		Clear();
	}
	template<typename T, AllocatorType Allocator>
	inline void DualList<T, Allocator>::Clear()
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
	inline bool DualList<T, Allocator>::Empty() const
	{
		return count == 0;
	}
	template<typename T, AllocatorType Allocator>
	inline uintMem DualList<T, Allocator>::Count() const
	{
		return count;
	}
	template<typename T, AllocatorType Allocator>
	template<typename ...Args> requires std::constructible_from<T, Args...>
	inline DualList<T, Allocator>::Iterator DualList<T, Allocator>::AddBefore(const Iterator& it, Args && ...args)
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (it.IsNull())
			Debug::Logger::LogFatal("Blaze Engine", "Iterator is null");
#endif		

		Node* next = it.node;
		Node* prev = next->prev;
		Node* node = (Node*)allocator.Allocate(sizeof(Node));
		next->prev = node;

		if (prev != nullptr)
			prev->next = node;

		std::construct_at(node, prev, next, std::forward<Args>(args)...);

		if (prev == nullptr)
			first = node;

		++count;

		return Iterator(node);
	}
	template<typename T, AllocatorType Allocator>
	template<typename ...Args> requires std::constructible_from<T, Args...>
	inline DualList<T, Allocator>::Iterator DualList<T, Allocator>::AddAfter(const Iterator& it, Args&& ...args)
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (it.IsNull())
			Debug::Logger::LogFatal("Blaze Engine", "Iterator is null");
#endif		

		Node* prev = it.node;
		Node* next = prev->next;
		Node* node = (Node*)allocator.Allocate(sizeof(Node));

		prev->next = node;
		if (next != nullptr)
			next->prev = node;
			
		std::construct_at(node, prev, next, std::forward<Args>(args)...);

		if (next == nullptr)
			last = node;

		++count;
		return Iterator(node);
	}
	template<typename T, AllocatorType Allocator>
	template<typename ... Args> requires std::constructible_from<T, Args...>
	inline DualList<T, Allocator>::template Iterator DualList<T, Allocator>::AddBack(Args&& ... args)
	{
		if (count == 0)
			AddFront(std::forward<Args>(args)...);
		else
		{
			last->next = (Node*)allocator.Allocate(sizeof(Node));
			std::construct_at(last->next, last, nullptr, std::forward<Args>(args)...);

			last = last->next;
			++count;
		}

		return Iterator(last);
	}
	template<typename T, AllocatorType Allocator>
	template<typename ... Args> requires std::constructible_from<T, Args...>
	inline DualList<T, Allocator>::template Iterator DualList<T, Allocator>::AddFront(Args&& ... args)
	{
		Node* second = first;

		first = (Node*)allocator.Allocate(sizeof(Node));
		std::construct_at(first, nullptr, second, std::forward<Args>(args)...);

		++count;

		if (second)
			second->prev = first;

		if (count == 1)
			last = first;

		return Iterator(first);
	}	
	template<typename T, AllocatorType Allocator>
	inline Result DualList<T, Allocator>::Erase(const Iterator& it)
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			return BLAZE_ERROR_RESULT("Blaze Engine", "Erasing from an empty list");

		if (it.IsNull())
			return BLAZE_ERROR_RESULT("Blaze Engine", "Iterator is null");
#endif

		Node* node = it.node;
		Node* prev = node->prev;	
		Node* next = node->next;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (node->iteratorCount != 0)
			Debug::Logger::LogWarning("Blaze Engine", "Erasing a list element while a iterator is referencing it");
#endif

		std::destroy_at(node);
		allocator.Free(node);

		if (prev != nullptr)
			prev->next = next;
		else
			first = next;

		if (next != nullptr)
			next->prev = prev;
		else
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
	inline Result DualList<T, Allocator>::EraseFirst()
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
	inline Result DualList<T, Allocator>::EraseLast()
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			return BLAZE_ERROR_RESULT("Blaze Engine", "Erasing from an empty list");
#endif
		Node* prev = last->prev;
		Node* node = last;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (node->iteratorCount != 0)
			Debug::Logger::LogWarning("Blaze Engine", "Erasing a list element while a iterator is referencing it");
#endif

		std::destroy_at(node);
		allocator.Free(node);

		last = prev;
		--count;

		if (count == 0)
		{
			first = nullptr;
			last = nullptr;
		}
		else if (count == 1)
		{
			first = last;
		}

		return Result();
	}
	template<typename T, AllocatorType Allocator>
	inline Result DualList<T, Allocator>::MoveBefore(const ConstIterator& it, const ConstIterator& relative)
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK					
		if (it.IsNull())
			return BLAZE_ERROR_RESULT("Blaze Engine", "Iterator is null");

		if (relative.IsNull())
			return BLAZE_ERROR_RESULT("Blaze Engine", "Iterator is null");
#endif
		Node* node = it.node;

		RemoveNodeFromSpotUnsafe(node);
		
		Node* next = relative.node;
		Node* prev = next->prev;

		if (prev != nullptr)
			prev->next = node;

		next->prev = node;

		node->prev = prev;
		node->next = next;

		return Result();
	}
	template<typename T, AllocatorType Allocator>
	inline Result DualList<T, Allocator>::MoveAfter(const ConstIterator& it, const ConstIterator& relative)
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK					
		if (it.IsNull())
			return BLAZE_ERROR_RESULT("Blaze Engine", "Iterator is null");

		if (relative.IsNull())
			return BLAZE_ERROR_RESULT("Blaze Engine", "Iterator is null");
#endif
		Node* node = it.node;

		RemoveNodeFromSpotUnsafe(node);
	
		Node* prev = relative.node;
		Node* next = prev->next;

		if (next != nullptr)
			next->prev = node;

		prev->next = node;

		node->prev = prev;
		node->next = next;

		return Result();
	}
	template<typename T, AllocatorType Allocator>
	inline Result DualList<T, Allocator>::MoveFront(const ConstIterator& it)
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK					
		if (it.IsNull())
			return BLAZE_ERROR_RESULT("Blaze Engine", "Iterator is null");				
#endif
		Node* node = it.node;

		RemoveNodeFromSpotUnsafe(node);
		
		node->next = first;
		node->prev = nullptr;

		if (count != 0)
		{
			first->prev = node;
			first = node;
		}
		else
		{
			first = node;
			last = node;
		}

		return Result();
	}
	template<typename T, AllocatorType Allocator>
	inline Result DualList<T, Allocator>::MoveBack(const ConstIterator& it)
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK					
		if (it.IsNull())
			return BLAZE_ERROR_RESULT("Blaze Engine", "Iterator is null");
#endif
		Node* node = it.node;

		RemoveNodeFromSpotUnsafe(node);

		node->next = nullptr;
		node->prev = last;

		if (count != 0)
		{
			last->next = node;
			last = node;
		}
		else
		{
			first = node;
			last = node;
		}

		return Result();
	}
	template<typename T, AllocatorType Allocator>
	template<typename C> requires std::invocable<C, const T&>&& std::same_as<std::invoke_result_t<C, const T&>, bool>
	inline Result DualList<T, Allocator>::EraseAll(const C& function)
	{
		if (first == nullptr)
			return Result();

		while (function(first->value))
		{
			CHECK_RESULT(EraseFirst());

			if (first == nullptr)
				return Result();
		}
		
		Node* curr = first->next;

		while (curr != nullptr)
		{
			Node* next = curr->next;

			if (function(curr->value))			
				CHECK_RESULT(Erase(Iterator(curr)));

			curr = next;			
		}

		return Result();
	}
	template<typename T, AllocatorType Allocator>
	template<typename C> requires std::invocable<C, const T&>&& std::same_as<std::invoke_result_t<C, const T&>, bool>
	inline Result DualList<T, Allocator>::EraseOne(const C& function)
	{
		if (count == 0)
			return Result();

		Node* it = first;

		if (function(it->value))
		{
			CHECK_RESULT(EraseFirst());
			return Result();
		}

		while (it != nullptr)
		{
			if (function(it->value))
			{
				CHECK_RESULT(Erase(Iterator(it)));
				break;
			}

			it = it->next;
		}

		return Result();
	}
	template<typename T, AllocatorType Allocator>
	inline void DualList<T, Allocator>::AppendBack(const DualList& list)
	{
		for (const auto& el : list)
			AddBack(el);
	}
	template<typename T, AllocatorType Allocator>
	inline void DualList<T, Allocator>::AppendBack(DualList&& list)
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
	inline void Blaze::DualList<T, Allocator>::AppendFront(const DualList& list)
	{
		for (const auto& el : list)
			AddFront(el);
	}
	template<typename T, AllocatorType Allocator>
	inline void Blaze::DualList<T, Allocator>::AppendFront(DualList&& list)
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
	inline T& DualList<T, Allocator>::First()
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			Debug::Logger::LogFatal("Blaze Engine", "DualList is empty");
#endif
		return first->value;
	}
	template<typename T, AllocatorType Allocator>
	inline const T& DualList<T, Allocator>::First() const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			Debug::Logger::LogFatal("Blaze Engine", "DualList is empty");
#endif
		return first->value;
	}
	template<typename T, AllocatorType Allocator>
	inline T& DualList<T, Allocator>::Last()
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			Debug::Logger::LogFatal("Blaze Engine", "DualList is empty");
#endif
		return last->value;
	}
	template<typename T, AllocatorType Allocator>
	inline const T& DualList<T, Allocator>::Last() const
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			Debug::Logger::LogFatal("Blaze Engine", "DualList is empty");
#endif
		return last->value;
	}
	template<typename T, AllocatorType Allocator>
	inline DualList<T, Allocator>::Iterator DualList<T, Allocator>::FirstIterator()
	{
		return Iterator(first);
	}
	template<typename T, AllocatorType Allocator>
	inline DualList<T, Allocator>::ConstIterator DualList<T, Allocator>::FirstIterator() const
	{
		return ConstIterator(first);
	}
	template<typename T, AllocatorType Allocator>
	inline DualList<T, Allocator>::Iterator DualList<T, Allocator>::LastIterator()
	{
		return Iterator(last);
	}
	template<typename T, AllocatorType Allocator>
	inline DualList<T, Allocator>::ConstIterator DualList<T, Allocator>::LastIterator() const
	{
		return ConstIterator(last);
	}
	template<typename T, AllocatorType Allocator>
	inline DualList<T, Allocator>::Iterator DualList<T, Allocator>::AheadIterator()
	{
		return Iterator();
	}
	template<typename T, AllocatorType Allocator>
	inline DualList<T, Allocator>::ConstIterator DualList<T, Allocator>::AheadIterator() const
	{
		return ConstIterator();
	}
	template<typename T, AllocatorType Allocator>
	inline DualList<T, Allocator>::Iterator DualList<T, Allocator>::BehindIterator()
	{
		return Iterator(nullptr);
	}
	template<typename T, AllocatorType Allocator>
	inline DualList<T, Allocator>::ConstIterator DualList<T, Allocator>::BehindIterator() const
	{
		return ConstIterator(nullptr);
	}
	template<typename T, AllocatorType Allocator>
	inline DualList<T, Allocator>& DualList<T, Allocator>::operator=(const DualList& other)
	{
		Clear();

		CopyUnsafe(other);

		return *this;
	}
	template<typename T, AllocatorType Allocator>
	inline DualList<T, Allocator>& DualList<T, Allocator>::operator=(DualList&& other) noexcept
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
	inline DualList<T, Allocator>::Node::Node(Node* prev, Node* next, Args&& ... args)
		: value(std::forward<Args>(args)...), prev(prev), next(next)
	{
	}
	template<typename T, AllocatorType Allocator>
	inline void DualList<T, Allocator>::CopyUnsafe(const DualList& other)
	{
		if (other.count == 0)
			return;

		first = (Node*)allocator.Allocate(sizeof(Node));
		std::construct_at(first, nullptr, nullptr, (const T&)*other.first);

		Node* itDst = first;
		Node* itSrc = other.first;

		while (itSrc->next != nullptr)
		{
			itSrc = itSrc->next;

			itDst->next = (Node*)allocator.Allocate(sizeof(Node));

			std::construct_at(itDst->next, itDst, nullptr, itSrc->value);
			
			itDst = itDst->next;
		}

		last = itDst;
		count = other.count;
	}

	template<typename T, AllocatorType Allocator>
	inline void DualList<T, Allocator>::RemoveNodeFromSpotUnsafe(Node* node)
	{
		Node* prev = node->prev;
		Node* next = node->next;

		if (prev != nullptr)
			prev->next = next;
		if (next != nullptr)
			next->prev = prev;		
	}

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