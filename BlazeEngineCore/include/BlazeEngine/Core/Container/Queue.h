#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/Memory/Allocator.h" 
#include <memory>

namespace Blaze
{
	template<typename T, AllocatorType Allocator = DefaultAllocator> requires IsConstructibleFrom<T, T&&>
	class BLAZE_API Queue
	{
	public:
		Queue();
		Queue(const Queue& other);
		Queue(Queue&& other) noexcept;
		~Queue();

		void Clear();
		uintMem Count() const;
		uintMem Capacity() const;
		bool Empty() const;

		template<typename ... Args> requires IsConstructibleFrom<T, Args...>
		void Push(Args&& ... args);
		T Pop();

		T& Front();
		const T& Front() const;
		T& Back();
		const T& Back() const;

		Queue& operator=(const Queue& other);
		Queue& operator=(Queue&& other) noexcept;
	private:
		T* ptr;
		T* end;
		T* head; //If head == end then there are no stored events
		T* tail; //If tail == head then there are no available slots for new events, otherwise tail is the next available place for storing
		BLAZE_ALLOCATOR_ATTRIBUTE Allocator allocator;

		void SetCapacity(uintMem newCount);

		static void CopyIntoEmptyQueue(Queue& dst, const Queue& src);
		static void CopyIntoEmptyQueueRaw(T* dstPtr, const Queue& src);

		void DestroyElements();
	};

	template<typename T, AllocatorType Allocator> requires IsConstructibleFrom<T, T&&>
	inline Queue<T, Allocator>::Queue()
		: ptr(nullptr), end(nullptr), head(nullptr), tail(nullptr)
	{
	}
	template<typename T, AllocatorType Allocator> requires IsConstructibleFrom<T, T&&>
	inline Queue<T, Allocator>::Queue(const Queue& other)
		: Queue()
	{
		CopyIntoEmptyQueue(*this, other);
	}
	template<typename T, AllocatorType Allocator> requires IsConstructibleFrom<T, T&&>
	inline Queue<T, Allocator>::Queue(Queue&& other) noexcept
		: ptr(std::exchange(other.ptr, nullptr)), end(std::exchange(other.end, nullptr)), head(std::exchange(other.head, nullptr)), tail(std::exchange(other.tail, nullptr))
	{
	}
	template<typename T, AllocatorType Allocator> requires IsConstructibleFrom<T, T&&>
	inline Queue<T, Allocator>::~Queue()
	{
		Clear();
	}
	template<typename T, AllocatorType Allocator> requires IsConstructibleFrom<T, T&&>
	inline void Queue<T, Allocator>::Clear()
	{
		if (Empty())
			return;

		DestroyElements();
		allocator.Free(ptr);

		ptr = nullptr;
		end = nullptr;
		head = nullptr;
		tail = nullptr;
	}
	template<typename T, AllocatorType Allocator> requires IsConstructibleFrom<T, T&&>
	inline uintMem Queue<T, Allocator>::Count() const
	{
		if (Empty())
			return 0;

		return head > tail ? end - head + tail - ptr : tail - head;
	}
	template<typename T, AllocatorType Allocator> requires IsConstructibleFrom<T, T&&>
	inline uintMem Queue<T, Allocator>::Capacity() const
	{
		return end - ptr;
	}
	template<typename T, AllocatorType Allocator> requires IsConstructibleFrom<T, T&&>
	inline bool Queue<T, Allocator>::Empty() const
	{
		return head == end;
	}
	template<typename T, AllocatorType Allocator> requires IsConstructibleFrom<T, T&&> 
	template<typename ...Args> requires IsConstructibleFrom<T, Args...>
	inline void Queue<T, Allocator>::Push(Args && ...args)
	{
		if (tail == head)
		{
			if (ptr == nullptr)
				SetCapacity(4);
			else
				SetCapacity((end - ptr) * 2);
		}
		
		std::construct_at(tail, std::forward<Args>(args)...);

		if (Empty())
			head = tail;

		++tail;

		if (tail == end)
			tail = ptr;
	}
	template<typename T, AllocatorType Allocator> requires IsConstructibleFrom<T, T&&>
	inline T Queue<T, Allocator>::Pop()
	{
		if (Empty())
		{
			BLAZE_LOG_FATAL_BASIC("Trying to pop an element from an empty queue");
			return { };
		}

		T element = std::move(*head);

		if constexpr (!IsTriviallyDestructible<T>)
			std::destroy_at(head);

		++head;

		if (Empty())
			head = ptr;

		if (head == tail)
		{
			head = end;
			tail = ptr;
		}
		
		uintMem count = Count();
		uintMem capacity = Capacity();
		if (count <= capacity / 4)
			SetCapacity(capacity / 2 > 4 ? capacity / 2 : 4);

		return element;
	}
	template<typename T, AllocatorType Allocator> requires IsConstructibleFrom<T, T&&>
	inline T& Queue<T, Allocator>::Front()
	{
		if (Empty())
			BLAZE_LOG_FATAL_BASIC("Calling Front() on an empty queue");

		return *head;
	}
	template<typename T, AllocatorType Allocator> requires IsConstructibleFrom<T, T&&>
	inline const T& Queue<T, Allocator>::Front() const
	{
		if (Empty())
			BLAZE_LOG_FATAL_BASIC("Calling Front() on an empty queue");

		return *head;
	}
	template<typename T, AllocatorType Allocator> requires IsConstructibleFrom<T, T&&>
	inline T& Queue<T, Allocator>::Back()
	{
		if (Empty())
			BLAZE_LOG_FATAL_BASIC("Calling Back() on an empty queue");

		if (tail == ptr)
			return *(end - 1);
		else
			return *(tail - 1);
	}
	template<typename T, AllocatorType Allocator> requires IsConstructibleFrom<T, T&&>
	inline const T& Queue<T, Allocator>::Back() const
	{
		if (Empty())
			BLAZE_LOG_FATAL_BASIC("Calling Back() on an empty queue");

		if (tail == ptr)
			return *(end - 1);
		else
			return *(tail - 1);
	}
	template<typename T, AllocatorType Allocator> requires IsConstructibleFrom<T, T&&>
	inline auto Queue<T, Allocator>::operator=(const Queue& other) -> Queue&
	{
		Clear();
		CopyIntoEmptyQueue(*this, other);
		return *this;
	}
	template<typename T, AllocatorType Allocator> requires IsConstructibleFrom<T, T&&>
	inline auto Queue<T, Allocator>::operator=(Queue&& other) noexcept -> Queue&
	{
		ptr = std::exchange(other.ptr, nullptr);
		end = std::exchange(other.end, nullptr);
		head = std::exchange(other.head, nullptr);
		tail = std::exchange(other.tail, nullptr);
		return *this;
	}
	template<typename T, AllocatorType Allocator> requires IsConstructibleFrom<T, T&&>
	inline void Queue<T, Allocator>::SetCapacity(uintMem newCapacity)
	{
		if (ptr == nullptr)
		{
			ptr = reinterpret_cast<T*>(allocator.Allocate(sizeof(T) * newCapacity));
			end = ptr + newCapacity;
			head = end;
			tail = ptr;
			return;
		}

		if (end - ptr == newCapacity)
			return;

		const uintMem count = Count();
		T* const newPtr = reinterpret_cast<T*>(allocator.Allocate(sizeof(T) * newCapacity));

		CopyIntoEmptyQueueRaw(newPtr, *this);
		DestroyElements();

		allocator.Free(ptr);

		ptr = newPtr;
		end = newPtr + newCapacity;
		head = count == 0 ? end : newPtr;
		tail = newPtr + count;
	}
	template<typename T, AllocatorType Allocator> requires IsConstructibleFrom<T, T&&>
	inline void Queue<T, Allocator>::CopyIntoEmptyQueue(Queue& dst, const Queue& src)
	{
		if (src.Empty())
			return;

		uintMem srcCapacity = src.end - src.ptr;

		dst.ptr = dst.allocator.Allocate(sizeof(T) * srcCapacity);
		dst.end = dst.ptr + srcCapacity;
	
		CopyIntoEmptyQueueRaw(dst.ptr, src);

		uintMem srcCount = src.Count();

		dst.head = srcCount == 0 ? dst.end : dst.ptr;
		dst.tail = dst.ptr + srcCount;
	}
	template<typename T, AllocatorType Allocator> requires IsConstructibleFrom<T, T&&>
	inline void Queue<T, Allocator>::CopyIntoEmptyQueueRaw(T* dstPtr, const Queue& src)
	{
		if (src.Empty())
			return;

		if constexpr (IsTriviallyCopyable<T>)
		{
			if (src.head > src.tail)
			{
				uintMem count1 = src.end - src.head;
				uintMem count2 = src.tail - src.ptr;
				memcpy(dstPtr, src.head, sizeof(T) * count1);
				memcpy(dstPtr + count1, src.ptr, sizeof(T) * count2);
			}
			else
				memcpy(dstPtr, src.head, sizeof(T) * (src.tail - src.head));
		}
		else
		{
			if (src.head > src.tail)
			{
				uintMem count1 = src.end - src.head;
				uintMem count2 = src.tail - src.ptr;

				for (auto i = 0; i < count1; ++i)
					std::construct_at(dstPtr + i, src.head[i]);
				for (auto i = 0; i < count2; ++i)
					std::construct_at(dstPtr + i + count1, src.ptr[i]);
			}
			else
				for (auto i = 0; i < src.tail - src.head; ++i)
					std::construct_at(dstPtr + i, src.head[i]);
		}
	}
	template<typename T, AllocatorType Allocator> requires IsConstructibleFrom<T, T&&>
	inline void Queue<T, Allocator>::DestroyElements()
	{
		if constexpr (IsTriviallyDestructible<T>)
			return;

		if (Empty())
			return;

		if (head > tail)
		{
			std::destroy(head, end);
			std::destroy(ptr, tail);
		}
		else
		{
			std::destroy(head, tail);
		}
	}
}