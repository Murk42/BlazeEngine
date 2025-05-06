#pragma once
#include "BlazeEngineCore/Event/EventHandler.h"
#include "BlazeEngineCore/Memory/Allocator.h"

namespace Blaze
{
	template<typename T, AllocatorType Allocator = DefaultAllocator, uint BucketSize = 8>
	class BLAZE_CORE_API QueueEventDispatcher : public EventDispatcher<T>
	{
	public:
		QueueEventDispatcher();

		void Call(const T& event) override;
		void ProcessQueue();

		bool IsEmpty() const;
	private:
		struct Bucket
		{
			byte elements[sizeof(T) / BucketSize];
			Bucket* next;
		};
		
		Bucket first;
		Bucket* last;
		uint lastBucketCount;
		BLAZE_ALLOCATOR_ATTRIBUTE Allocator allocator;
	};
	template<typename T, AllocatorType Allocator, uint BucketSize>
	inline QueueEventDispatcher<T, Allocator, BucketSize>::QueueEventDispatcher()
		: lastBucketCount(0), last(&first)
	{
		first.next = nullptr;
	}

	template<typename T, AllocatorType Allocator, uint BucketSize>
	inline void QueueEventDispatcher<T, Allocator, BucketSize>::Call(const T& event)
	{		
		if (lastBucketCount == BucketSize)
		{
			lastBucketCount = 0;
			last->next = allocator.Allocate(sizeof(Bucket));
			last = last->next;
			last->next = nullptr;
		}

		T* element = (T*)(last->elements + lastBucketCount * sizeof(T));
		std::construct_at(element, std::move(event));
		++lastBucketCount;
	}
	template<typename T, AllocatorType Allocator, uint BucketSize>
	inline void QueueEventDispatcher<T, Allocator, BucketSize>::ProcessQueue()
	{					
		T* it = first->elements;

		if (first->next == nullptr)
		{
			for (uint i = 0; i < lastBucketCount; ++i)
			{
				EventDispatcher::Call(it[i]);
				it[i].~T();
			}

			lastBucketCount = 0;

			return;
		}

		for (uint i = 0; i < BucketSize; ++i)
			EventDispatcher::Call(it[i]);

		Bucket* current = first->next;

		while (current->next != nullptr)
		{
			it = current->elements;

			for (uint i = 0; i < BucketSize; ++i, ++it)
			{
				EventDispatcher::Call(*it);

				it->~T();
			}			

			allocator.Free(current);
			current = current->next;
		}		

		it = current->elements;

		for (uint i = 0; i < lastBucketCount; ++i, ++it)
		{
			EventDispatcher::Call(*it);
			it->~T();
		}

		allocator.Free(current);

		lastBucketCount = 0;
		first.next = nullptr;
		last = &first;
	}
	template<typename T, AllocatorType Allocator, uint BucketSize>
	inline bool QueueEventDispatcher<T, Allocator, BucketSize>::IsEmpty() const
	{
		return first->next != nullptr || lastBucketCount != 0;
	}
}