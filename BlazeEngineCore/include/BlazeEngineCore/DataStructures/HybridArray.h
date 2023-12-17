#pragma once

namespace Blaze
{
	template<typename T, typename Allocator = std::allocator<T>>
	class HybridArray
	{
		struct BucketHeader;
	public:		
		class Iterator
		{
			const HybridArray* container;
			BucketHeader* bucket;
			T* ptr;

			Iterator(const HybridArray* container, BucketHeader* bucket, T* ptr);
		public:
			Iterator();
			Iterator(const Iterator& other);

			Iterator& operator++();
			Iterator operator++(int);

			bool operator==(const Iterator& other) const;
			bool operator!=(const Iterator& other) const;

			T& operator*();
			T* operator->();

			Iterator& operator=(const Iterator& other);

			template<typename, typename>
			friend class HybridArray;
		};

		HybridArray();
		~HybridArray();

		void SetBucketElementCount(uint count);

		template<typename ... Args>
		T* Allocate(Args&& ... args);
		Result Free(T* ptr);

		uint Count() const { return elementCount; }

		void Clear();

		void Free(const Iterator& it);

		Iterator begin() const;
		Iterator end() const;

	private:
		//expects startBitIndex and endBitIndex to be a multiple of 8
		static inline size_t CountRightOnes(uint8* buffer, size_t size, size_t firstBitIndex);
		static inline size_t CountRightZeros(uint8* buffer, size_t size, size_t firstBitIndex);

		static inline void FlipBit(uint8* buffer, uint index);
		static inline bool GetBit(uint8* buffer, uint index);		

		struct Element
		{
			BucketHeader* bucket;
			T data;
		};

		struct BucketHeader
		{
			BucketHeader* next;
			BucketHeader* prev;
			uint elementCount;
			uint8* state;
			Element* elements;
		};

		uint elementCount;
		uint bucketStateBufferSize;
		uint bucketSize;
		uint bucketElementCount;
		BucketHeader* first;
		BucketHeader* last;
		BucketHeader* firstFull;
		BLAZE_ALLOCATOR_ATTRIBUTE Allocator allocator;

		template<typename ... Args>
		Element* BucketHeaderAllocate(BucketHeader* bucket, Args&& ... args);
		void BucketHeaderFree(Element* ptr);
		Element* FirstInBucketHeader(BucketHeader* bucket) const;

		void Increment(BucketHeader*& bucket, T*& ptr) const;

		BucketHeader* InsertAtBeginningBucketHeader();
		void EraseBucketHeader(BucketHeader* bucket);

		void MarkFirstBucketHeaderFull();
		void MarkBucketHeaderNotFull(BucketHeader* bucket);
		void MoveBefore(BucketHeader* bucket, BucketHeader* before);
		void MoveAfter(BucketHeader* bucket, BucketHeader* after);

		void CheckValidity();
	};

	template<typename T, typename Allocator>
	inline HybridArray<T, Allocator>::Iterator::Iterator(const HybridArray* container, BucketHeader* bucket, T* ptr)
		: container(container), bucket(bucket), ptr(ptr)
	{

	}
	template<typename T, typename Allocator>
	inline HybridArray<T, Allocator>::Iterator::Iterator()
		: container(nullptr), bucket(nullptr), ptr(nullptr)
	{

	}
	template<typename T, typename Allocator>
	inline HybridArray<T, Allocator>::Iterator::Iterator(const Iterator& other)
		: container(other.container), bucket(other.bucket), ptr(ptr)
	{

	}
	template<typename T, typename Allocator>
	inline HybridArray<T, Allocator>::Iterator& HybridArray<T, Allocator>::Iterator::operator++()
	{
		container->Increment(bucket, ptr);
		return *this;
	}
	template<typename T, typename Allocator>
	inline HybridArray<T, Allocator>::Iterator HybridArray<T, Allocator>::Iterator::operator++(int)
	{
		Iterator out = *this;
		++* this;
		return out;
	}

	template<typename T, typename Allocator>
	inline bool HybridArray<T, Allocator>::Iterator::operator==(const Iterator& other) const
	{
		return container == other.container && ptr == other.ptr;
	}
	template<typename T, typename Allocator>
	inline bool HybridArray<T, Allocator>::Iterator::operator!=(const Iterator& other) const
	{
		return container != other.container || ptr != other.ptr;
	}

	template<typename T, typename Allocator>
	inline T& HybridArray<T, Allocator>::Iterator::operator*()
	{
		return *ptr;
	}
	template<typename T, typename Allocator>
	inline T* HybridArray<T, Allocator>::Iterator::operator->()
	{
		return ptr;
	}

	template<typename T, typename Allocator>
	inline HybridArray<T, Allocator>::Iterator& HybridArray<T, Allocator>::Iterator::operator=(const Iterator& other)
	{
		container = other.container;
		bucket = other.bucket;
		ptr = other.ptr;
		return *this;
	}

	template<typename T, typename Allocator>
	inline HybridArray<T, Allocator>::HybridArray()
		: first(nullptr), last(nullptr), firstFull(nullptr), elementCount(0), bucketSize(0), bucketStateBufferSize(0)
	{
		SetBucketElementCount(64);
	}
	template<typename T, typename Allocator>
	inline HybridArray<T, Allocator>::~HybridArray()
	{
		Clear();
	}
	template<typename T, typename Allocator>
	inline void HybridArray<T, Allocator>::SetBucketElementCount(uint count)
	{
		count = (count + 7) / 8 * 8;

		bucketStateBufferSize = count / 8;
		bucketElementCount = count;

		bucketSize = sizeof(BucketHeader) + bucketElementCount * (sizeof(Element)) + bucketStateBufferSize;
	}
	template<typename T, typename Allocator>
	inline Result HybridArray<T, Allocator>::Free(T* ptr)
	{
		Element* element = structptr(Element, data, ptr);
		BucketHeader* bucket = element->bucket;

		bool wasFull = bucket->elementCount == bucketElementCount;

		BucketHeaderFree(element);
		--elementCount;

		if (wasFull)
			MarkBucketHeaderNotFull(bucket);

		if (bucket->elementCount == 0)		
			EraseBucketHeader(bucket);

		std::destroy_at(ptr);
#ifdef _DEBUG
		memset(ptr, 0, sizeof(T));
#endif

		if (bucket->elementCount == 0)
			allocator.Free(bucket);		

		return Result();
	}
	template<typename T, typename Allocator>
	inline void HybridArray<T, Allocator>::Clear()
	{
		Allocator a;
		for (auto& element : *this)
			std::allocator_traits<Allocator>::destroy(a, &element);

		BucketHeader* bucket = first;
		while (bucket != nullptr)
		{
			BucketHeader* old = bucket;
			bucket = bucket->next;

			allocator.Free(old);
		}

		elementCount = 0;
		first = nullptr;
		last = nullptr;
		firstFull = nullptr;
	}
	template<typename T, typename Allocator>
	inline void HybridArray<T, Allocator>::Free(const Iterator& it)
	{
		Free(it.ptr);
	}
	template<typename T, typename Allocator>
	inline HybridArray<T, Allocator>::Iterator HybridArray<T, Allocator>::begin() const
	{
		if (first == 0)
			return end();
		return Iterator(this, first, (T*)&FirstInBucketHeader(first)->data);
	}
	template<typename T, typename Allocator>
	inline HybridArray<T, Allocator>::Iterator HybridArray<T, Allocator>::end() const
	{
		return Iterator(this, nullptr, nullptr);
	}

	//expects startBitIndex and endBitIndex to be a multiple of 8
	template<typename T, typename Allocator>
	inline size_t HybridArray<T, Allocator>::CountRightOnes(uint8* buffer, size_t size, size_t firstBitIndex)
	{
		if (firstBitIndex * 8 == size)
			return 0;

		if (firstBitIndex * 8 > size)
			throw;

		size_t count = 0;
		uint _count;

		buffer += firstBitIndex / 8;
		size -= firstBitIndex / 8;
		firstBitIndex %= 8;


		if (firstBitIndex != 0)
		{
			_count = std::countr_one<uint8>(*buffer >> firstBitIndex);
			if (_count != firstBitIndex) return count + _count;
			count += _count;
			++buffer;
			--size;
		}

		while (size >= 8)
		{
			_count = std::countr_one<uint64>(*(uint64*)buffer);
			if (_count < 64) return count + _count;
			count += _count;
			buffer += 8;
			size -= 8;
		}

		if (size >= 4)
		{
			_count = std::countr_one<uint32>(*(uint32*)buffer);
			if (_count < 32) return count + _count;
			count += _count;
			buffer += 4;
			size -= 4;
		}

		if (size >= 2)
		{
			_count = std::countr_one<uint16>(*(uint16*)buffer);
			if (_count < 16) return count + _count;
			count += _count;
			buffer += 2;
			size -= 2;
		}

		if (size >= 1)
		{
			_count = std::countr_one<uint8>(*(uint8*)buffer);
			if (_count < 8) return count + _count;
			count += _count;
			buffer += 1;
			size -= 1;
		}
		return count;
	}
	template<typename T, typename Allocator>
	inline size_t HybridArray<T, Allocator>::CountRightZeros(uint8* buffer, size_t size, size_t firstBitIndex)
	{
		if (firstBitIndex == size * 8)
			return 0;

		if (firstBitIndex > size * 8)
			throw;

		size_t count = 0;
		uint _count;

		buffer += firstBitIndex / 8;
		size -= firstBitIndex / 8;
		firstBitIndex %= 8;

		if (firstBitIndex != 0)
		{
			_count = std::countr_zero<uint8>(*buffer >> firstBitIndex | (uint8)(0b100000000i64 >> firstBitIndex));
			if (_count != 8 - firstBitIndex)
				return count + _count;
			count += _count;
			++buffer;
			--size;
		}

		while (size >= 8)
		{
			_count = std::countr_zero<uint64>(*(uint64*)buffer);
			if (_count < 64) return count + _count;
			count += _count;
			buffer += 8;
			size -= 8;
		}

		if (size >= 4)
		{
			_count = std::countr_zero<uint32>(*(uint32*)buffer);
			if (_count < 32) return count + _count;
			count += _count;
			buffer += 4;
			size -= 4;
		}

		if (size >= 2)
		{
			_count = std::countr_zero<uint16>(*(uint16*)buffer);
			if (_count < 16) return count + _count;
			count += _count;
			buffer += 2;
			size -= 2;
		}

		if (size >= 1)
		{
			_count = std::countr_zero<uint8>(*(uint8*)buffer);
			if (_count < 8) return count + _count;
			count += _count;
			buffer += 1;
			size -= 1;
		}
		return count;
	}
	template<typename T, typename Allocator>
	inline void HybridArray<T, Allocator>::FlipBit(uint8* buffer, uint index)
	{
		uint8 mask = 1i8 << (index % 8);
		uint offset = index / 8;
		uint8& byte = *(buffer + offset);
		byte ^= mask;
	}
	template<typename T, typename Allocator>
	inline bool HybridArray<T, Allocator>::GetBit(uint8* buffer, uint index)
	{
		uint8 mask = 1i8 << (index % 8);
		uint offset = index / 8;
		uint8& byte = *(buffer + offset);

		return (bool)(byte & mask);
	}
	template<typename T, typename Allocator>
	inline void HybridArray<T, Allocator>::BucketHeaderFree(Element* ptr)
	{
		BucketHeader* bucket = ptr->bucket;
		uint index = ptr - bucket->elements;

		if (GetBit(bucket->state, index) == false)
			Debug::Logger::LogError("Blaze Engine", "Trying to free a element in a HybridArray that wasn't allocated yet");

		FlipBit(bucket->state, index);

		bucket->elementCount--;		
	}
	template<typename T, typename Allocator>
	inline HybridArray<T, Allocator>::Element* HybridArray<T, Allocator>::FirstInBucketHeader(BucketHeader* bucket) const
	{
		return bucket->elements + CountRightZeros(bucket->state, bucketStateBufferSize, 0);
	}
	template<typename T, typename Allocator>
	inline void HybridArray<T, Allocator>::Increment(BucketHeader*& bucket, T*& ptr) const
	{
		Element* element = structptr(Element, data, ptr);
		uint index = element - bucket->elements;

		index += 1 + CountRightZeros(bucket->state, bucketStateBufferSize, index + 1);

		if (index == bucketElementCount)
		{
			bucket = bucket->next;

			if (bucket == nullptr)
				ptr = nullptr;
			else
				ptr = (T*)&(FirstInBucketHeader(bucket)->data);
		}
		else
			ptr = (T*)&bucket->elements[index].data;
	}
	template<typename T, typename Allocator>
	inline HybridArray<T, Allocator>::BucketHeader* HybridArray<T, Allocator>::InsertAtBeginningBucketHeader()
	{
		BucketHeader* newFirst = (BucketHeader*)allocator.Allocate(bucketSize);

		newFirst->next = first;
		newFirst->prev = nullptr;
		newFirst->elementCount = 0;
		newFirst->state = (uint8*)newFirst + sizeof(BucketHeader);
		newFirst->elements = (Element*)((uint8*)newFirst + sizeof(BucketHeader) + bucketStateBufferSize);

		memset(newFirst->state, 0, bucketStateBufferSize);

		if (first == nullptr) //If list is empty			
			last = newFirst;
		else
			first->prev = newFirst;

		first = newFirst;

		CheckValidity();

		return newFirst;
	}
	template<typename T, typename Allocator>
	inline void HybridArray<T, Allocator>::EraseBucketHeader(BucketHeader* bucket)
	{
		if (bucket == last)
			last = bucket->prev;
		else
			bucket->next->prev = bucket->prev;

		if (bucket == first)
			first = bucket->next;
		else
			bucket->prev->next = bucket->next;		

		CheckValidity();
	}
	template<typename T, typename Allocator>
	inline void HybridArray<T, Allocator>::MarkFirstBucketHeaderFull()
	{
		BucketHeader* bucket = first;

		if (firstFull == nullptr)
			MoveAfter(bucket, last);
		else
			MoveBefore(bucket, firstFull);

		firstFull = bucket;

		CheckValidity();
	}
	template<typename T, typename Allocator>
	inline void HybridArray<T, Allocator>::MarkBucketHeaderNotFull(BucketHeader* bucket)
	{
		if (bucket == firstFull)
			firstFull = bucket->next;
		else
			MoveBefore(bucket, firstFull);

		CheckValidity();
	}
	template<typename T, typename Allocator>
	inline void HybridArray<T, Allocator>::MoveBefore(BucketHeader* bucket, BucketHeader* before)
	{
		if (bucket == before || bucket == before->prev)
			return;

		if (bucket->prev != nullptr)
			bucket->prev->next = bucket->next;
		else
			first = bucket->next;

		if (bucket->next != nullptr)
			bucket->next->prev = bucket->prev;
		else
			last = bucket->prev;

		if (before->prev == nullptr)
			first = bucket;

		bucket->prev = before->prev;
		bucket->next = before;

		if (before->prev != nullptr)
			before->prev->next = bucket;
		before->prev = bucket;
	}
	template<typename T, typename Allocator>
	inline void HybridArray<T, Allocator>::MoveAfter(BucketHeader* bucket, BucketHeader* after)
	{
		if (bucket == after || after->next == bucket)
			return;

		if (bucket->prev != nullptr)
			bucket->prev->next = bucket->next;
		else
			first = bucket->next;

		if (bucket->next != nullptr)
			bucket->next->prev = bucket->prev;
		else
			last = bucket->prev;

		if (after->next == nullptr)
			last = bucket;

		bucket->prev = after;
		bucket->next = after->next;

		if (after->next != nullptr)
			after->next->prev = bucket;
		after->next = bucket;
	}
	template<typename T, typename Allocator>
	inline void HybridArray<T, Allocator>::CheckValidity()
	{
#ifdef _DEBUG
		if (first == nullptr && last == nullptr)
			return;

		if (first == nullptr || last == nullptr)
			throw;

		if (first->prev != nullptr)
			throw;

		if (last->next != nullptr)
			throw;

		BucketHeader* it = first;
		bool passed = false;
		while (it != nullptr)
		{
			if (it == firstFull)
				passed = true;

			if (passed != (it->elementCount == bucketElementCount))
				throw;

			if (it == last)
				return;

			it = it->next;
		}

		if (it != last)
			throw;
#endif
	}
	template<typename T, typename Allocator>
	template<typename ...Args>
	inline T* HybridArray<T, Allocator>::Allocate(Args && ...args)
	{
		if (firstFull == first)
			InsertAtBeginningBucketHeader();

		elementCount++;
		T* element = &BucketHeaderAllocate(first, std::forward<Args>(args)...)->data;

		if (first->elementCount == bucketElementCount)
			MarkFirstBucketHeaderFull();
		
		std::construct_at(element, std::forward<Args>(args)...);		
		
		return element;
	}
	template<typename T, typename Allocator>
	template<typename ...Args>
	inline HybridArray<T, Allocator>::Element* HybridArray<T, Allocator>::BucketHeaderAllocate(BucketHeader* bucket, Args && ...args)
	{
		uint index = CountRightOnes(bucket->state, bucketStateBufferSize, 0);
		FlipBit(bucket->state, index);

		Element* element = bucket->elements + index;
		element->bucket = bucket;

		bucket->elementCount++;		
		return element;
	}
}