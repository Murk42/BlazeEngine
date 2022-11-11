#pragma once
#include <memory>

namespace Blaze
{	
	template<typename, typename>
	class List;
	template<typename T>
	class ListNode
	{
	public:
		T value;
		ListNode* next;

		ListNode()
			: value(), next(nullptr)
		{
		}
		ListNode(const T& value, ListNode* next = nullptr)
			: value(value), next(next)
		{
		}
		ListNode(T&& value, ListNode* next = nullptr)
			: value(std::move(value)), next(next)
		{
		}
	};
	template<typename T, typename Allocator>
	class ListIterator
	{
		List<T, Allocator>* list;
		ListNode<T>* node;

		ListIterator(List<T, Allocator>* list, ListNode<T>* node)
			: list(list), node(node)
		{
		}
	public:
		using value_type = T;

		ListIterator()
			: list(nullptr), node(nullptr)
		{
		}
		ListIterator(const ListIterator& i)
			: list(i.list), node(i.node)
		{
		}

		ListIterator& operator++()
		{
			if (node == nullptr)
				throw std::exception("Iterator out of range");

			node = node->next;
			return *this;
		}
		ListIterator operator++(int)
		{
			ListIterator copy = *this;

			if (node == nullptr)
				throw std::exception("Iterator out of range");

			node = node->next;
			return copy;
		} 

		T& operator*()
		{
			return node->value;;
		}
		const T& operator*() const
		{
			return node->value;
		}
		T& operator->()
		{
			return node->value;
		}
		const T& operator->() const
		{
			return node->value;
		}

		bool operator==(const ListIterator& i) const
		{
			return node == i.node && list == i.list;
		}
		bool operator!=(const ListIterator& i) const
		{
			return node != i.node;
		}

		ListIterator& operator=(const ListIterator& i)
		{
			list = i.list;
			node = i.node;
			return *this;
		}

		template<typename, typename>
		friend class List;
	};
	template<typename T, typename Allocator = std::allocator<ListNode<T>>>
	class List
	{	
		ListNode<T>* first;
		ListNode<T>* last;
		size_t count;
	public:
		using Node = ListNode<T>;
		using Iterator = ListIterator<T, Allocator>;
		using value_type = T;

		List()
			: first(nullptr), last(nullptr), count(0)
		{
		}
		List(const List& list)
			: first(nullptr), last(nullptr), count(0)
		{
			if (list.count == 0)
				return;

			Allocator a;
			first = a.allocate(1);
			new (first) Node (list.first->value);

			Node* it = list.first->next;
			Node* it2 = first;
			while (it != nullptr)
			{
				it2->next = a.allocate(1);
				new (it2->next) Node(it->value);

				it = it->next;
				it2 = it2->next;
			}

			count = list.count;
		}
		List(List&& list) noexcept
		{
			first = list.first;
			last = list.last;
			count = list.count;
			list.first = nullptr;
			list.last = nullptr;
			list.count = 0;
		}
		~List()
		{
			Clear();
		}

		void Clear()
		{
			if (count == 0)
				return;
			Allocator a;

			Node* it = first;

			while (it != nullptr)
			{
				Node* next = it->next;
				it->~Node();
				a.deallocate(it, 1);
				it = next;
			}

			count = 0;
			first = 0;
			last = 0;
		}
		bool Empty() const
		{
			return count == 0;
		}

		Iterator begin()
		{
			return Iterator(this, first);
		}
		Iterator begin() const
		{
			return Iterator((List*)this, first);
		}
		Iterator end()
		{
			return Iterator(this, nullptr);
		}
		Iterator end() const
		{
			return Iterator((List*)this, nullptr);
		}

		size_t Count() const
		{
			return count;
		}

		Iterator AddBack(const T& value)
		{
			if (count == 0)
				AddFront(value);
			else
			{
				Allocator a;
				last->next = a.allocate(1);
				new (last->next) Node (value);
				last = last->next;
				++count;
			}

			return Iterator(this, last);
		}
		Iterator AddFront(const T& value)
		{
			Allocator a;
			Node* second = first;

			first = a.allocate(1);
			new (first) Node(value, second);
			++count;

			if (count == 1)
			{
				last = first;
			}

			return Iterator(this, first);
		}

		void EraseAfter(const Iterator& it)
		{
			if (it.node->next == nullptr)
				throw std::exception("Invalid iterator");

			Allocator a;

			Node* after = it.node->next->next;
			it.node->next->~Node();
			a.deallocate(it.node->next, 1);
			it.node->next = after;
			--count;

			if (count == 0)
			{
				first = nullptr;
				last = nullptr;
			}
		}
		Iterator PopFirst()
		{
			if (count == 0)
				throw std::exception("Erasing from a empty list");

			Allocator a;

			Node* next = first->next;
			first->~Node();			
			a.deallocate(first, 1);
			first = next;
			--count;

			if (count == 0)
			{
				first = nullptr;
				last = nullptr;
			}

			return { this, first };
		}

		List& operator=(const List& l)
		{
			Clear();

			if (l.count == 0)
			{
				first = nullptr;
				last = nullptr;
				count = 0;
				return *this;
			}

			Allocator a;
			first = a.allocate(1);
			new (first) Node(l.first->value);

			Node* it = l.first->next;
			Node* it2 = first;
			while (it != nullptr)
			{
				it2->next = a.allocate(1);
				new (it2->next) Node(it->value);				

				it = it->next;
				it2 = it2->next;
			}

			count = l.count;

			return *this;
		}
		List& operator=(List&& l) noexcept
		{
			Clear();
			first = l.first;
			last = l.last;
			count = l.count;
			l.first = nullptr;
			l.last = nullptr;
			l.count = 0;
			return *this;
		}
	};
}