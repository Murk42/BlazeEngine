#pragma once
#include "VirtualList.h"

namespace Blaze
{
	template<typename VirtualList>
	inline VirtualListIterator<VirtualList>::VirtualListIterator()
		: node(nullptr)
	{
	}
	template<typename VirtualList>
	template<IsConvertibleToVirtualListIterator<VirtualListIterator<VirtualList>> T>
	inline VirtualListIterator<VirtualList>::VirtualListIterator(const T& i)
		: node(i.node)
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (node != nullptr)
			++node->iteratorCount;
#endif
	}
	template<typename VirtualList>
	inline bool VirtualListIterator<VirtualList>::IsNull() const
	{
		return node == nullptr;
	}
	template<typename VirtualList>
	inline VirtualListIterator<VirtualList>& VirtualListIterator<VirtualList>::operator++()
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
	template<typename VirtualList>
	inline VirtualListIterator<VirtualList> VirtualListIterator<VirtualList>::operator++(int)
	{
		VirtualListIterator copy = *this;
		++copy;
		return copy;
	}
	template<typename VirtualList>
	inline typename VirtualListIterator<VirtualList>::template ValueBaseType& VirtualListIterator<VirtualList>::operator*() const requires (!std::same_as<ValueBaseType, void>)
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (node == nullptr)
			Debug::Logger::LogFatal("Blaze Engine", "Dereferencing a null iterator");
#endif

		return *node->valueBase;
	}

	template<typename VirtualList>
	inline typename VirtualListIterator<VirtualList>::template ValueBaseType* VirtualListIterator<VirtualList>::operator->() const requires (!std::same_as<ValueBaseType, void>)
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (node == nullptr)
			Debug::Logger::LogFatal("Blaze Engine", "Dereferencing a null iterator");
#endif

		return node->valueBase;
	}

	template<typename VirtualList>
	template<typename Value> requires InsertableToVirtualList<Value, typename VirtualList::template ValueBaseType>
	inline typename VirtualListIterator<VirtualList>::template MatchValueBaseConst<Value>* VirtualListIterator<VirtualList>::GetValue() const 
	{
		if (node == nullptr)
			return nullptr;

		auto nodeRead = dynamic_cast<typename VirtualList::template Node<Value>*>(node);

		if (nodeRead == nullptr)
			return nullptr;

		return &nodeRead->value;
	}
	template<typename VirtualList>
	template<IsComparableToVirtualListIterator<VirtualListIterator<VirtualList>> T>
	inline bool VirtualListIterator<VirtualList>::operator==(const T& i) const
	{
		return node == i.node;
	}
	template<typename VirtualList>
	template<IsComparableToVirtualListIterator<VirtualListIterator<VirtualList>> T>
	inline bool VirtualListIterator<VirtualList>::operator!=(const T& i) const
	{
		return node != i.node;
	}
	template<typename VirtualList>
	template<IsConvertibleToVirtualListIterator<VirtualListIterator<VirtualList>> T>
	inline VirtualListIterator<VirtualList>& VirtualListIterator<VirtualList>::operator=(const T& i)
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
	template<typename VirtualList>
	inline VirtualListIterator<VirtualList>::VirtualListIterator(Node* node)
		: node(node)
	{
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

		NodeHeader* it = first;

		while (it != nullptr)
		{
			NodeHeader* next = it->next;

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
	template<typename Derived, typename ... Args> requires InsertableToVirtualList<Derived, T>&& std::constructible_from<Derived, Args...>
	inline VirtualList<T, Allocator>::Iterator VirtualList<T, Allocator>::AddAfter(const Iterator& it, Args&& ...args)
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (it.IsNull())
			Debug::Logger::LogFatal("Blaze Engine", "Iterator is null");
#endif		

		NodeHeader* prev = it.node;
		NodeHeader* next = prev->next;
		Node<Derived>* nodeReal = (Node<Derived>*)allocator.Allocate(sizeof(Node<Derived>));
		NodeHeader* node = (NodeHeader*)nodeReal;
		prev->next = node;
		std::construct_at(nodeReal, next, std::forward<Args>(args)...);

		if (next == nullptr)
			last = node;

		++count;
		return Iterator(node);
	}
	template<typename T, AllocatorType Allocator>
	template<typename Derived, typename ... Args> requires InsertableToVirtualList<Derived, T>&& std::constructible_from<Derived, Args...>
	inline VirtualList<T, Allocator>::template Iterator VirtualList<T, Allocator>::AddBack(Args&& ... args)
	{
		if (count == 0)
			AddFront<Derived, Args...>(std::forward<Args>(args)...);
		else
		{
			Node<Derived>* node = (Node<Derived>*)allocator.Allocate(sizeof(Node<Derived>));
			last->next = (NodeHeader*)node;
			std::construct_at(node, nullptr, std::forward<Args>(args)...);

			last = last->next;
			++count;
		}

		return Iterator(last);
	}
	template<typename T, AllocatorType Allocator>
	template<typename Derived, typename ... Args> requires InsertableToVirtualList<Derived, T>&& std::constructible_from<Derived, Args...>
	inline VirtualList<T, Allocator>::template Iterator VirtualList<T, Allocator>::AddFront(Args&& ... args)
	{
		NodeHeader* second = first;

		Node<Derived>* node = (Node<Derived>*)allocator.Allocate(sizeof(Node<Derived>));
		first = (NodeHeader*)node;
		std::construct_at(node, second, std::forward<Args>(args)...);

		++count;

		if (count == 1)
			last = first;

		return Iterator(first);
	}
	template<typename T, AllocatorType Allocator>
	inline Result VirtualList<T, Allocator>::EraseAfter(const Iterator& it)
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			return BLAZE_ERROR_RESULT("Blaze Engine", "Erasing from an empty list");

		if (it.IsNull())
			return BLAZE_ERROR_RESULT("Blaze Engine", "Iterator is null");
#endif

		NodeHeader* prev = it.node;
		NodeHeader* node = prev->next;

#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (node == nullptr)
			return BLAZE_ERROR_RESULT("Blaze Engine", "Iterator is the last in the list");
#endif

		NodeHeader* next = node->next;

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
	inline Result VirtualList<T, Allocator>::EraseFirst()
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (count == 0)
			return BLAZE_ERROR_RESULT("Blaze Engine", "Erasing from an empty list");
#endif
		NodeHeader* next = first->next;
		NodeHeader* node = first;

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
	inline Result VirtualList<T, Allocator>::EraseAll(const C& function)
	{
		if (count == 0)
			return Result();

		NodeHeader* prev = first;
		NodeHeader* curr = prev->next;

		if (function(first->value))
			CHECK_RESULT(EraseFirst())

			while (curr != nullptr)
			{
				NodeHeader* next = curr->next;

				if (function(curr->value))
				{
					CHECK_RESULT(EraseAfter(Iterator(prev)));

					curr = next;
				}
				else
				{
					prev = curr;
					curr = next;
				}
			}

		return Result();
	}
	template<typename T, AllocatorType Allocator>
	template<typename C> requires std::invocable<C, T&>&& std::same_as<std::invoke_result_t<C, T&>, bool>
	inline Result VirtualList<T, Allocator>::EraseOne(const C& function)
	{
		if (count == 0)
			return Result();

		NodeHeader* prev = first;
		NodeHeader* curr = prev->next;

		if (function(first->value))
		{
			CHECK_RESULT(EraseFirst())
				return Result();
		}

		while (curr != nullptr)
		{
			if (function(curr->value))
			{
				CHECK_RESULT(EraseAfter(Iterator(prev)));
				return Result();
			}

			prev = curr;
			curr = curr->next;
		}

		return Result();
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
	template<typename T, AllocatorType Allocator>
	inline VirtualList<T, Allocator>::NodeHeader::NodeHeader(NodeHeader* next, T* valueBase)
		: next(next), valueBase(valueBase)
	{
	}
	template<typename T, AllocatorType Allocator>
	inline VirtualList<T, Allocator>::NodeHeader::~NodeHeader()
	{
	}	
	template<typename T, AllocatorType Allocator>
	template<typename Value> requires InsertableToVirtualList<Value, T>
	template<typename ...Args> requires std::constructible_from<Value, Args...>
	inline VirtualList<T, Allocator>::Node<Value>::Node(typename VirtualList<T, Allocator>::template NodeHeader* next, Args && ...args)
		: NodeHeader(next, &value), value(std::forward<Args>(args)...)
	{
	}
	template<typename T, AllocatorType Allocator>	
	template<typename Value> requires InsertableToVirtualList<Value, T>
	inline VirtualList<T, Allocator>::Node<Value>::~Node()
	{
	}	

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