#include "Queue.h"
#pragma once

namespace Blaze
{
	template<typename T>
	inline QueueIterator<T>::QueueIterator()
		: ptr(nullptr), queue(nullptr)
	{

	}
	template<typename T>
	inline QueueIterator<T>::QueueIterator(const QueueIterator& other)
		: ptr(other.ptr), queue(other.queue)
	{

	}
	template<typename T>
	inline QueueIterator<T>::QueueIterator(StoredType* ptr, const QueueBase<T>* queue)
		: ptr(ptr), queue(queue)
	{

	}
	template<typename T>
	inline QueueIterator<T>::~QueueIterator()
	{

	}
	template<typename T>
	inline RemoveReference<T>* QueueIterator<T>::Ptr() const
	{
		return ptr;
	}
	template<typename T>
	inline bool QueueIterator<T>::IsNull() const
	{
		return ptr == nullptr;
	}
	template<typename T>
	inline T& QueueIterator<T>::operator*() const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (IsNull())
			BLAZE_LOG_FATAL("Dereferencing a null iterator");
#endif

		return *ptr;
	}
	template<typename T>
	inline RemoveReference<T>* QueueIterator<T>::operator->() const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (IsNull())
			BLAZE_LOG_FATAL("Dereferencing a null iterator");
#endif

		return ptr;
	}
	template<typename T>
	inline QueueIterator<T>& QueueIterator<T>::operator++()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (IsNull())
			BLAZE_LOG_FATAL("Incrementing a null iterator");
#endif

		if (ptr == queue->begin - 1) //check if pointing "ahead" of the queue
			ptr = queue->head;
		else
		{
			++ptr;

			if (ptr == queue->end)
				ptr = queue->begin; //"wrap" the pointer

			if (ptr == queue->tail)
				ptr = queue->end; //point "behind" the queue
		}

		return *this;
	}
	template<typename T>
	inline QueueIterator<T> QueueIterator<T>::operator++(int)
	{
		QueueIterator copy = *this;
		++copy;
		return copy;
	}
	template<typename T>
	inline QueueIterator<T>& QueueIterator<T>::operator--()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (IsNull())
			BLAZE_LOG_FATAL("Incrementing a null iterator");
#endif

		if (ptr == queue->end) //check if pointing "behind" the queue
		{
			if (queue->tail == queue->begin)
				ptr = queue->end - 1;
			else
				ptr = queue->tail - 1;
		}
		else
		{ 
			if (ptr == queue->head)
				ptr = queue->begin - 1; //point "ahead" of the queue
			else if (ptr == queue->ptr)
				ptr = queue->end - 1; //"wrap" the pointer
			else
				--ptr;
		}

		return *this;
	}
	template<typename T>
	inline QueueIterator<T> QueueIterator<T>::operator--(int)
	{
		QueueIterator copy = *this;
		--copy;
		return copy;
	}
	template<typename T>
	inline bool QueueIterator<T>::operator==(const QueueIterator& other) const
	{
		return ptr == other.ptr;
	}
	template<typename T>
	inline bool QueueIterator<T>::operator!=(const QueueIterator& other) const
	{
		return ptr != other.ptr;
	}
	template<typename T>
	inline QueueIterator<T>::operator QueueIterator<const T>() const
	{
		return QueueIterator<const T>(ptr, queue);
	}
	template<typename T>
	inline QueueIterator<T>::operator const RemoveReference<T>* () const
	{
		return ptr;
	}
	template<typename T>
	inline QueueIterator<T>::operator RemoveReference<T>* () const requires (!IsConstType<T>)
	{ 
		return ptr;
	}
	template<typename T>
	inline QueueIterator<T>& QueueIterator<T>::operator=(const QueueIterator& other)
	{
		ptr = other.ptr;
		queue = other.queue;
		return *this;
	}

	template<typename T>
	inline QueueBase<T>::QueueBase()
		: begin(nullptr), end(nullptr), head(nullptr), tail(nullptr)
	{
	}
	template<typename T>
	inline QueueBase<T>::QueueBase(QueueBase&& other) noexcept
		: begin(std::exchange(other.begin, nullptr)), end(std::exchange(other.end, nullptr)), head(std::exchange(other.head, nullptr)), tail(std::exchange(other.tail, nullptr))
	{
	}
	template<typename T>
	inline QueueBase<T>::~QueueBase()
	{
	}
	template<typename T>
	inline bool QueueBase<T>::Empty() const
	{
		return head == nullptr;
	}
	template<typename T>
	inline uintMem QueueBase<T>::Count() const
	{
		return head >= tail ? end - head + tail - begin : tail - head;
	}
	template<typename T>
	inline uintMem QueueBase<T>::ReservedCount() const
	{
		return end - begin;
	}
	template<typename T>
	inline T& QueueBase<T>::Front()
	{
		if (Empty())
			BLAZE_LOG_FATAL_BASIC("Calling Front() on an empty queue");

		return *head;
	}
	template<typename T>
	inline const T& QueueBase<T>::Front() const
	{
		if (Empty())
			BLAZE_LOG_FATAL_BASIC("Calling Front() on an empty queue");

		return *head;
	}
	template<typename T>
	inline T& QueueBase<T>::Back()
	{
		if (Empty())
			BLAZE_LOG_FATAL_BASIC("Calling Back() on an empty queue");

		return tail == begin ? *(end - 1) : *(tail - 1);
	}
	template<typename T>
	inline const T& QueueBase<T>::Back() const
	{
		if (Empty())
			BLAZE_LOG_FATAL_BASIC("Calling Back() on an empty queue");

		return tail == begin ? *(end - 1) : *(tail - 1);
	}
	template<typename T>
	inline auto QueueBase<T>::FirstIterator() -> Iterator 
	{
		if (Empty())
			return Iterator();

		return Iterator(head, this);
	}
	template<typename T>
	inline auto QueueBase<T>::FirstIterator() const -> ConstIterator 
	{
		if (Empty())
			return Iterator();

		return ConstIterator(head, this);
	}
	template<typename T>
	inline auto QueueBase<T>::LastIterator() -> Iterator 
	{
		if (Empty())
			return Iterator();

		if (tail == begin)
			return Iterator(end - 1, this);
		else
			return Iterator(tail - 1, this);
	}
	template<typename T>
	inline auto QueueBase<T>::LastIterator() const -> ConstIterator 
	{
		if (Empty())
			return Iterator();

		if (tail == begin)
			return ConstIterator(end - 1, this);
		else
			return ConstIterator(tail - 1, this);
	}
	template<typename T>
	inline auto QueueBase<T>::AheadIterator() -> Iterator 
	{
		if (Empty())
			return Iterator();

		return Iterator(begin - 1, this);
	}
	template<typename T>
	inline auto QueueBase<T>::AheadIterator() const -> ConstIterator 
	{
		if (Empty())
			return ConstIterator();

		return ConstIterator(begin - 1, this);
	}
	template<typename T>
	inline auto QueueBase<T>::BehindIterator() -> Iterator 
	{
		if (Empty())
			return Iterator();

		return Iterator(end, this);
	}
	template<typename T>
	inline auto QueueBase<T>::BehindIterator() const -> ConstIterator 
	{
		if (Empty())
			return ConstIterator();

		return ConstIterator(end, this);
	}
	template<typename T>
	inline auto QueueBase<T>::operator=(QueueBase&& other) noexcept -> QueueBase&
	{
		begin = std::exchange(other.begin, nullptr);
		end = std::exchange(other.end, nullptr);
		head = std::exchange(other.head, nullptr);
		tail = std::exchange(other.tail, nullptr);
		return *this;
	}

	template<typename T, AllocatorType Allocator>
	inline Queue<T, Allocator>::Queue()
		: QueueBase<T>()
	{
	}
	template<typename T, AllocatorType Allocator>
	inline Queue<T, Allocator>::Queue(const Queue& other)
		: Queue()
	{
		CopyUnsafe(*this, other);
	}
	template<typename T, AllocatorType Allocator>
	inline Queue<T, Allocator>::Queue(Queue&& other) noexcept
		: QueueBase<T>(std::move(other))
	{
	}
	template<typename T, AllocatorType Allocator>
	inline Queue<T, Allocator>::~Queue()
	{
		Clear();
	}
	template<typename T, AllocatorType Allocator>
	inline void Queue<T, Allocator>::Clear()
	{
		DestroyElements();
		allocator.Free(QueueBase<T>::begin);

		QueueBase<T>::begin = nullptr;
		QueueBase<T>::end = nullptr;
		QueueBase<T>::head = nullptr;
		QueueBase<T>::tail = nullptr;
	}
	template<typename T, AllocatorType Allocator>
	template<typename ...Args> requires IsConstructibleFrom<T, Args...>
	inline void Queue<T, Allocator>::Push(Args && ...args) requires IsConstructibleFrom<T, T&&>
	{
		if (QueueBase<T>::tail == QueueBase<T>::head)
			if (QueueBase<T>::Empty())
				ReserveExactly(4);
			else
				ReserveExactly(QueueBase<T>::ReservedCount() * 2);

		std::construct_at(QueueBase<T>::tail, std::forward<Args>(args)...);

		if (QueueBase<T>::head == nullptr)
			QueueBase<T>::head = QueueBase<T>::tail;

		++QueueBase<T>::tail;

		if (QueueBase<T>::tail == QueueBase<T>::end)
			QueueBase<T>::tail = QueueBase<T>::begin;
	}
	template<typename T, AllocatorType Allocator>
	inline T Queue<T, Allocator>::Pop() requires IsConstructibleFrom<T, T&&>
	{
		if (QueueBase<T>::Empty())
		{
			BLAZE_LOG_FATAL_BASIC("Trying to pop an element from an empty queue");
		}

		T element = std::move(*QueueBase<T>::head);

		if constexpr (!IsTriviallyDestructible<T>)
			std::destroy_at(QueueBase<T>::head);

		++QueueBase<T>::head;

		if (QueueBase<T>::head == QueueBase<T>::end)
			QueueBase<T>::head = QueueBase<T>::begin;

		if (QueueBase<T>::head == QueueBase<T>::tail)
		{
			QueueBase<T>::head = nullptr;
			QueueBase<T>::tail = QueueBase<T>::begin;
		}

		uintMem count = QueueBase<T>::Count();
		uintMem reservedCount = QueueBase<T>::ReservedCount();

		if (count <= reservedCount / 4)
			ReserveExactly(reservedCount / 2 > 4 ? reservedCount / 2 : 4);

		return element;
	}
	template<typename T, AllocatorType Allocator>
	inline void Queue<T, Allocator>::ReserveExactly(uintMem newReservedCount) requires IsConstructibleFrom<T, T&&>
	{
		if (QueueBase<T>::end - QueueBase<T>::begin == newReservedCount)
			return;

		const uintMem count = QueueBase<T>::Count();
		T* const newPtr = static_cast<QueueBase<T>::StoredType*>(allocator.Allocate(sizeof(QueueBase<T>::StoredType) * newReservedCount));

		MoveConstructElements(newPtr, *this);
		DestroyElements();

		allocator.Free(QueueBase<T>::begin);

		QueueBase<T>::begin = newPtr;
		QueueBase<T>::end = newPtr + newReservedCount;
		QueueBase<T>::head = count == 0 ? nullptr : newPtr;
		QueueBase<T>::tail = newPtr + count;
	}
	template<typename T, AllocatorType Allocator>
	inline void Queue<T, Allocator>::ReserveAdditional(uintMem additionalReservedCount) requires IsConstructibleFrom<T, T&&>
	{
		ReserveExactly(QueueBase<T>::ReservedCount() + additionalReservedCount);
	}
	template<typename T, AllocatorType Allocator>
	inline auto Queue<T, Allocator>::operator=(const Queue& other) -> Queue&
	{
		Clear();
		CopyUnsafe(*this, other);
		return *this;
	}
	template<typename T, AllocatorType Allocator>
	inline auto Queue<T, Allocator>::operator=(Queue&& other) noexcept -> Queue&
	{
		this->QueueBase<T>::operator=(std::move(other));
		allocator = std::move(other.allocator);
		return *this;
	}
	template<typename T, AllocatorType Allocator>
	inline void Queue<T, Allocator>::CopyUnsafe(Queue& dst, const Queue& src)
	{
		if (src.Empty())
			return;

		uintMem srcCount = src.ReservedCount();

		dst.begin = static_cast<QueueBase<T>::StoredType*>(dst.allocator.Allocate(sizeof(QueueBase<T>::StoredType) * srcCount));
		dst.end = dst.begin + srcCount;
		dst.head = dst.begin;
		dst.tail = dst.begin;

		CopyConstructElements(dst.begin, src);
	}
	template<typename T, AllocatorType Allocator>
	inline void Queue<T, Allocator>::CopyConstructElements(T* dstPtr, const Queue& src)
	{
		if (src.Empty())
			return;

		if constexpr (IsTriviallyAssignable<T&, const T&>)
		{
			if (src.head >= src.tail)
			{
				uintMem count1 = src.end - src.head;
				uintMem count2 = src.tail - src.begin;
				memcpy(dstPtr, src.head, sizeof(T) * count1);
				memcpy(dstPtr + count1, src.begin, sizeof(T) * count2);
			}
			else
				memcpy(dstPtr, src.head, sizeof(T) * (src.tail - src.head));
		}
		else
		{
			if (src.head >= src.tail)
			{
				uintMem count1 = src.end - src.head;
				uintMem count2 = src.tail - src.begin;

				for (auto i = 0; i < count1; ++i)
					std::construct_at(dstPtr + i, src.head[i]);
				for (auto i = 0; i < count2; ++i)
					std::construct_at(dstPtr + i + count1, src.begin[i]);
			}
			else
				for (auto i = 0; i < src.tail - src.head; ++i)
					std::construct_at(dstPtr + i, src.head[i]);
		}
	}
	template<typename T, AllocatorType Allocator>
	inline void Queue<T, Allocator>::MoveConstructElements(T* dstPtr, Queue& src)
	{
		if (src.Empty())
			return;

		if constexpr (IsTriviallyAssignable<T&, T&&>)
		{
			if (src.head >= src.tail)
			{
				uintMem count1 = src.end - src.head;
				uintMem count2 = src.tail - src.begin;
				memcpy(dstPtr, src.head, sizeof(T) * count1);
				memcpy(dstPtr + count1, src.begin, sizeof(T) * count2);
			}
			else
				memcpy(dstPtr, src.head, sizeof(T) * (src.tail - src.head));
		}
		else
		{
			if (src.head >= src.tail)
			{
				uintMem count1 = src.end - src.head;
				uintMem count2 = src.tail - src.begin;

				for (auto i = 0; i < count1; ++i)
					std::construct_at(dstPtr + i, std::move(src.head[i]));
				for (auto i = 0; i < count2; ++i)
					std::construct_at(dstPtr + i + count1, std::move(src.begin[i]));
			}
			else
				for (auto i = 0; i < src.tail - src.head; ++i)
					std::construct_at(dstPtr + i, std::move(src.head[i]));
		}
	}
	template<typename T, AllocatorType Allocator>
	inline void Queue<T, Allocator>::DestroyElements()
	{
		if constexpr (IsTriviallyDestructible<T>)
			return;

		if (QueueBase<T>::Empty())
			return;

		if (QueueBase<T>::head >= QueueBase<T>::tail)
		{
			std::destroy(QueueBase<T>::head, QueueBase<T>::end);
			std::destroy(QueueBase<T>::begin, QueueBase<T>::tail);
		}
		else
		{
			std::destroy(QueueBase<T>::head, QueueBase<T>::tail);
		}
	}
}