#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/Memory/Allocator.h" 
#include "BlazeEngine/Core/Container/ArrayView.h"
#include "BlazeEngine/Core/Container/RawArray.h"
#include <memory>

namespace Blaze
{
	template<typename T>
	class QueueBase;

	template<typename T>
	class BLAZE_API QueueIterator
	{
	public:
		using ValueType = T;
		using StoredType = Conditional<IsReferenceType<T>, const ReferenceContainer<RemoveReference<T>>, T>;

		//STD compatibility
		using value_type = T;

		QueueIterator();
		QueueIterator(const QueueIterator& other);
		QueueIterator(StoredType* ptr, const QueueBase<T>* queue);
		~QueueIterator();

		RemoveReference<T>* Ptr() const;
		bool IsNull() const;

		T& operator*() const;
		RemoveReference<T>* operator->() const;

		QueueIterator& operator++();
		QueueIterator operator++(int);
		QueueIterator& operator--();
		QueueIterator operator--(int);

		bool operator==(const QueueIterator& other) const;
		bool operator!=(const QueueIterator& other) const;

		operator QueueIterator<const T>() const;
		operator const RemoveReference<T>* () const;
		operator RemoveReference<T>* () const requires (!IsConstType<T>);

		QueueIterator& operator=(const QueueIterator& other);
	private:
		/*
			If ptr is equal to queue->end the iterator is pointing "behind" the queue.
			If ptr is equal to queue->begin - 1 the iterator is pointing "ahead" of the queue.
		*/
		StoredType* ptr; 
		const QueueBase<T>* queue;
	};

	template<typename T>
	class BLAZE_API QueueBase
	{
	public:
		using Iterator = QueueIterator<T>;
		using ConstIterator = QueueIterator<const T>;
		using ValueType = T;
		using StoredType = Conditional<IsReferenceType<T>, ReferenceContainer<RemoveReference<T>>, T>;

		bool Empty() const;
		uintMem Count() const;
		uintMem ReservedCount() const;

		T& Front();
		const T& Front() const;
		T& Back();
		const T& Back() const;

		Iterator FirstIterator();
		ConstIterator FirstIterator() const;
		Iterator LastIterator();
		ConstIterator LastIterator() const;
		Iterator AheadIterator();
		ConstIterator AheadIterator() const;
		Iterator BehindIterator();
		ConstIterator BehindIterator() const;

		QueueBase& operator=(QueueBase&& other) noexcept;
	protected:
		StoredType* begin;
		StoredType* end;
		StoredType* head;
		StoredType* tail;

		QueueBase();
		QueueBase(QueueBase&& other) noexcept;
		~QueueBase();

		friend class QueueIterator<T>;
	};

	template<typename T, AllocatorType Allocator = DefaultAllocator>
	class BLAZE_API Queue : public QueueBase<T>
	{
	public:
		Queue();
		Queue(const Queue& other);
		Queue(Queue&& other) noexcept;
		~Queue();

		void Clear();

		template<typename ... Args> requires IsConstructibleFrom<T, Args...>
		void Push(Args&& ... args) requires IsConstructibleFrom<T, T&&>;
		T Pop() requires IsConstructibleFrom<T, T&&>;

		void ReserveExactly(uintMem newReservedCount) requires IsConstructibleFrom<T, T&&>;
		void ReserveAdditional(uintMem additionalReservedCount) requires IsConstructibleFrom<T, T&&>;

		Queue& operator=(const Queue& other);
		Queue& operator=(Queue&& other) noexcept;
	private:
		BLAZE_ALLOCATOR_ATTRIBUTE Allocator allocator;

		/*
			Previous elements wont be freed
		*/
		static void CopyUnsafe(Queue& dst, const Queue& src);
		static void CopyConstructElements(T* dstPtr, const Queue& src);
		static void MoveConstructElements(T* dstPtr, Queue& src);

		void DestroyElements();
	};
}

#include "BlazeEngine/Core/Container/QueueImpl.h"