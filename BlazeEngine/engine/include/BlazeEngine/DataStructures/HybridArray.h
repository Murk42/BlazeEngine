#pragma once
#include <utility>

namespace Blaze
{
	template<typename T, typename Allocator = std::allocator<T>>
	class HybridArray
	{		
		//expects startBitIndex and endBitIndex to be a multiple of 8
		static inline size_t CountRightOnes(uint8* buffer, size_t size, size_t firstBitIndex)
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
		static inline size_t CountRightZeros(uint8* buffer, size_t size, size_t firstBitIndex)
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

		static inline void FlipBit(uint8* buffer, uint index)
		{
			uint8 mask = 1i8 << (index % 8);
			uint offset = index / 8;
			uint8& byte = *(buffer + offset);
			byte ^= mask;			
		}
		static inline bool GetBit(uint8* buffer, uint index)
		{
			uint8 mask = 1i8 << (index % 8);
			uint offset = index / 8;
			uint8& byte = *(buffer + offset);

			return (bool)(byte & mask);
		}

		struct BucketHeader;

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

		template<typename ... Args>
		Element* BucketHeaderAllocate(BucketHeader* bucket, Args&& ... args)
		{			
			uint index = CountRightOnes(bucket->state, bucketStateBufferSize, 0);
			FlipBit(bucket->state, index);			
			
			Element* element = bucket->elements + index;
			element->bucket = bucket;

			bucket->elementCount++;

			Allocator a;
			std::allocator_traits<Allocator>::construct(a, &element->data, std::forward<Args>(args)...);
			return element;
		}
		void BucketHeaderFree(Element* ptr)
		{
			BucketHeader* bucket = ptr->bucket;
			uint index = ptr - bucket->elements;			

			if (GetBit(bucket->state, index) == false)
				Logger::ProcessLog(BLAZE_FATAL_LOG("Blaze Engine", "Trying to free a element in a HybridArray that wasn't allocated yet"));
			
			FlipBit(bucket->state, index);									

			bucket->elementCount--;
						
			Allocator a;
			std::allocator_traits<Allocator>::destroy(a, &ptr->data);
#ifdef _DEBUG
			memset(&ptr->data, 0, sizeof(T));
#endif
		}
		Element* FirstInBucketHeader(BucketHeader* bucket) const
		{
			return bucket->elements + CountRightZeros(bucket->state, bucketStateBufferSize, 0);
		}
		//Element* LastInBucketHeader(BucketHeader* bucket) const
		//{
		//	return bucket->elements + bucketStateBufferSize - 1 - std::countl_zero(bucket->flags);
		//}

		void Increment(BucketHeader*& bucket, T*& ptr) const
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
		//void Decrement(BucketHeader*& bucket, T*& ptr) const
		//{
		//	Element* element = structptr(Element, data, ptr);
		//	uint index = element - bucket->elements;
		//
		//	FlagType mask = (((FlagType)1) << index) - 1;
		//	index = std::countl_zero(bucket->flags & mask);
		//
		//	if (index > bucketStateBufferSize)
		//	{
		//		bucket = bucket->prev;				
		//
		//		if (bucket == nullptr)
		//		{					
		//			ptr = nullptr;
		//		}
		//		else
		//			ptr = (T*)&(LastInBucketHeader(bucket)->data);
		//	}
		//	else
		//		ptr = (T*)&bucket->elements[index].data;
		//}

		BucketHeader* InsertAtBeginningBucketHeader()
		{
			BucketHeader* newFirst = (BucketHeader*)Memory::Allocate(bucketSize);

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
		void EraseBucketHeader(BucketHeader* bucket)
		{
			if (bucket == last)
				last = bucket->prev;
			else
				bucket->next->prev = bucket->prev;

			if (bucket == first)
				first = bucket->next;
			else
				bucket->prev->next = bucket->next;

			Memory::Free(bucket);

			CheckValidity();
		}

		void MarkFirstBucketHeaderFull()
		{		
			BucketHeader* bucket = first;

			if (firstFull == nullptr)			
				MoveAfter(bucket, last);		
			else			
				MoveBefore(bucket, firstFull);			

			firstFull = bucket;

			CheckValidity();
		}
		void MarkBucketHeaderNotFull(BucketHeader* bucket) 
		{			
			if (bucket == firstFull)			
				firstFull = bucket->next;
			else
				MoveBefore(bucket, firstFull);			

			CheckValidity();
		}
		void MoveBefore(BucketHeader* bucket, BucketHeader* before)
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
		void MoveAfter(BucketHeader* bucket, BucketHeader* after)
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

		void CheckValidity()
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
	public:
		HybridArray()
			: first(nullptr), last(nullptr), firstFull(nullptr), elementCount(0), bucketSize(0), bucketStateBufferSize(0)
		{
			SetBucketElementCount(64);
		}
		~HybridArray()
		{
			Clear();
		}

		void SetBucketElementCount(uint count)
		{
			count = (count + 7) / 8 * 8;

			bucketStateBufferSize = count / 8;
			bucketElementCount = count;

			bucketSize = sizeof(BucketHeader) + bucketElementCount * (sizeof(Element)) + bucketStateBufferSize;
		}

		template<typename ... Args>
		T* Allocate(Args&& ... args)
		{
			if (firstFull == first)
				InsertAtBeginningBucketHeader();			

			elementCount++;
			T* element = &BucketHeaderAllocate(first, std::forward<Args>(args)...)->data;

			if (first->elementCount == bucketElementCount) 
				MarkFirstBucketHeaderFull();						

			return element;
		}
		Result Free(T* ptr)
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

			return Result();
		}

		uint Count() const { return elementCount; }

		void Clear()
		{
			Allocator a;
			for (auto& element : *this)
				std::allocator_traits<Allocator>::destroy(a, &element);				
			
			BucketHeader* bucket = first;
			while (bucket != nullptr)
			{
				BucketHeader* old = bucket;
				bucket = bucket->next;

				Memory::Free(old);				
			}								 				

			elementCount = 0;
			first = nullptr;
			last = nullptr;
			firstFull = nullptr;
		}

		class Iterator
		{
			const HybridArray* container;
			BucketHeader* bucket;
			T* ptr;

			Iterator(const HybridArray* container, BucketHeader* bucket, T* ptr)
				: container(container), bucket(bucket), ptr(ptr)
			{

			}
		public:
			Iterator() 
				: container(nullptr), bucket(nullptr), ptr(nullptr)
			{

			}
			Iterator(const Iterator& other)
				: container(other.container), bucket(other.bucket), ptr(ptr)
			{

			}

			Iterator& operator++()
			{
				container->Increment(bucket, ptr);
				return *this;
			}
			Iterator operator++(int)
			{
				Iterator out = *this;
				++*this;
				return out;
			}
			//Iterator& operator--()
			//{
			//	container->Decrement(bucket, ptr);
			//	return *this;
			//}
			//Iterator operator--(int)
			//{
			//	Iterator out = *this;
			//	--*this;
			//	return out;
			//}

			bool operator==(const Iterator& other) const
			{
				return container == other.container && ptr == other.ptr;
			}
			bool operator!=(const Iterator& other) const
			{
				return container != other.container || ptr != other.ptr;
			}

			T& operator*()
			{
				return *ptr;
			}
			T* operator->()
			{
				return ptr;
			}

			Iterator& operator=(const Iterator& other)
			{
				container = other.container;
				bucket = other.bucket;				
				ptr = other.ptr;
				return *this;
			}

			template<typename, typename>
			friend class HybridArray;
		};

		void Free(const Iterator& it)
		{
			Free(it.ptr);
		}

		Iterator begin() const
		{
			if (first == 0)
				return end();
			return Iterator(this, first, (T*)&FirstInBucketHeader(first)->data);
		}
		Iterator end() const
		{
			return Iterator(this, nullptr, nullptr);
		}
	};
}