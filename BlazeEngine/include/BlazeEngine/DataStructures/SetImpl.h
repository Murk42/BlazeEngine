#include "Set.h"
#pragma once

namespace Blaze
{
	template<typename Set>
	inline SetIterator<Set>::SetIterator()
		: set(nullptr), node(nullptr)
	{
	}
	template<typename Set>
	template<IsConvertibleToSetIterator<SetIterator<Set>> T>
	inline SetIterator<Set>::SetIterator(const T& other)
		: set(other.set), node(other.node)
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (node != nullptr)
			++node->iteratorCount;
#endif
	}
	template<typename Set>
	inline bool SetIterator<Set>::IsNull() const
	{
		return node == nullptr;
	}
	template<typename Set>
	inline SetIterator<Set>& SetIterator<Set>::operator++()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK				
		if (node == nullptr)
			Debug::Logger::LogFatal("Blaze Engine", "Incrementing a null iterator");
#endif		

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (node != nullptr)
			--node->iteratorCount;
#endif

		if (node->next != nullptr)
		{
			node = node->next;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
			--node->iteratorCount;
#endif

			return *this;
		}

		auto* bucket = set->GetBucketFromHash(node->hash);

		++bucket;
		while (bucket->head == nullptr)
		{
			++bucket;

			if (bucket - set->buckets == set->bucketCount)
			{
				node = nullptr;
				return *this;
			}
		}		

		node = bucket->head;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		--node->iteratorCount;
#endif
		return *this;
	}
	template<typename Set>
	inline SetIterator<Set> SetIterator<Set>::operator++(int)
	{
		auto copy = *this;
		++copy;
		return copy;
	}
	template<typename Set>
	inline SetIterator<Set>& SetIterator<Set>::operator--()
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK			
		if (node == nullptr)
			Debug::Logger::LogFatal("Blaze Engine", "Decrementing a null iterator");
#endif			

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		--node->iteratorCount;
#endif

		if (node->prev != nullptr)
		{
			node = node->prev;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
			++node->iteratorCount;
#endif

			return *this;
		}

		auto* bucket = set->GetBucketFromHash(node->hash);

		--bucket;
		while (bucket->tail == nullptr)
		{
			--bucket;

			if (bucket == set->buckets)
			{
				node = nullptr;
				return *this;
			}
		}


		node = bucket->tail;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		++node->iteratorCount;
#endif

		return *this;
	}
	template<typename Set>
	inline SetIterator<Set> SetIterator<Set>::operator--(int)
	{
		auto copy = *this;
		--copy;
		return copy;
	}
	template<typename Set>
	inline SetIterator<Set>::ValueType& SetIterator<Set>::operator*() const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (node == nullptr)
		{
			Debug::Logger::LogFatal("Blaze Engine", "Dereferencing a null iterator");
			return *(ValueType*)nullptr;
		}
#endif

		return node->value;
	}
	template<typename Set>
	inline SetIterator<Set>::ValueType* SetIterator<Set>::operator->() const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (node == nullptr)
		{
			Debug::Logger::LogFatal("Blaze Engine", "Dereferencing a null iterator");
			return nullptr;
		}
#endif

		return &node->value;
	}
	template<typename Set>
	template<IsComparableToSetIterator<SetIterator<Set>> T>
	inline bool SetIterator<Set>::operator==(const T& other) const
	{
		return node == other.node;
	}
	template<typename Set>
	template<IsComparableToSetIterator<SetIterator<Set>> T>
	inline bool SetIterator<Set>::operator!=(const T& other) const
	{
		return node != other.node;
	}
	template<typename Set>
	template<IsConvertibleToSetIterator<SetIterator<Set>> T>
	inline SetIterator<Set>& SetIterator<Set>::operator=(const T& other)
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (node != nullptr)
			--node->iteratorCount;
#endif
		set = other.set;
		node = other.node;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (node != nullptr)
			++node->iteratorCount;
#endif
	}
	template<typename Set>
	inline SetIterator<Set>::SetIterator(Node* node, Set* set)
		: set(set), node(node)
	{

	}

	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Set<Value, Hasher, Allocator>::Set()
		: buckets(nullptr), bucketCount(0), count(0)
	{
		CheckForRehash();
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Set<Value, Hasher, Allocator>::Set(const Set& other)
		: buckets(nullptr), bucketCount(0), count(0)
	{
		CopyUnsafe(other);
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Set<Value, Hasher, Allocator>::Set(Set&& other) noexcept
		: bucketCount(other.bucketCount), buckets(other.buckets), count(other.count)
	{
		other.bucketCount = 0;
		other.buckets = nullptr;
		other.count = 0;
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Set<Value, Hasher, Allocator>::Set(const ArrayView<Value>& other)
		: buckets(nullptr), bucketCount(0), count(0)
	{
		for (const auto& el : other)
			Insert(el);
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Set<Value, Hasher, Allocator>::Set(const std::initializer_list<Value>& other)
		: buckets(nullptr), bucketCount(0), count(0)
	{
		for (const auto& el : other)
			Insert(el);
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Set<Value, Hasher, Allocator>::~Set()
	{
		Clear();
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline void Set<Value, Hasher, Allocator>::Clear()
	{
		Bucket* begin = buckets;
		Bucket* end = buckets + bucketCount;
		for (Bucket* it = begin; it != end; ++it)
		{
			Node* node = it->head;
			while (node != nullptr)
			{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK				
				if (node->iteratorCount)
					Debug::Logger::LogWarning("Blaze Engine", "Clearing a set while a iterator is referencing it");
#endif
				Node* next = node->next;
				std::destroy_at(node);
				Memory::Free(node);
				node = next;
			}
		}

		Memory::Free(buckets);

		buckets = nullptr;
		bucketCount = 0;
		count = 0;
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline bool Set<Value, Hasher, Allocator>::Empty() const
	{
		return count == 0;
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Set<Value, Hasher, Allocator>::Iterator Set<Value, Hasher, Allocator>::Find(const Value& value)
	{
		return FindWithHint(value, Hash(value));
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Set<Value, Hasher, Allocator>::ConstIterator Set<Value, Hasher, Allocator>::Find(const Value& value) const
	{
		return ConstIterator(FindWithHint(value, Hash(value)));
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline bool Set<Value, Hasher, Allocator>::Contains(const Value& value) const
	{
		uint hash = Hash(value);
		auto* bucket = GetBucketFromHash(hash);

		if (bucket == nullptr) //Not a error. If we were calling Contains on an empty set it shouldn't return an error
			return false;

		Node* node = bucket->head;

		if (node == nullptr)
			return false;

		while (node->value != value)
		{
			node = node->next;

			if (node == nullptr)
				return false;
		}

		return true;
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Set<Value, Hasher, Allocator>::InsertResult Set<Value, Hasher, Allocator>::Insert(const Value& value)
	{
		return InsertWithHint(value, Hash(value));
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline bool Set<Value, Hasher, Allocator>::Erase(const Value& value)
	{
		Bucket* bucket = GetBucketFromHash(Hash(value));

		if (bucket == nullptr)
			return false;			

		auto it = FindWithHintUnsafe(value, bucket);

		if (it.IsNull())
			return false;

		EraseNodeUnsafe(bucket, it.node);

		return true;
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline bool Set<Value, Hasher, Allocator>::Erase(const Iterator& iterator)
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (iterator.IsNull())
			return BLAZE_ERROR_RESULT("Blaze Engine", "Trying to erase a set value with a null iterator");
#endif

		Bucket* bucket = GetBucketFromHash(iterator.node->hash);

		if (bucket == nullptr)
			return false;		

		EraseNodeUnsafe(bucket, iterator.node);

		return true;
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline uint Set<Value, Hasher, Allocator>::Count() const
	{
		return count;
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Set<Value, Hasher, Allocator>::Iterator Set<Value, Hasher, Allocator>::FirstIterator()
	{
		if (bucketCount == 0)
			return Iterator();
		
		for (uint i = 0; i < bucketCount; ++i)
			if (buckets[i].head != nullptr)
				return Iterator(buckets[i].head, this);
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Set<Value, Hasher, Allocator>::ConstIterator Set<Value, Hasher, Allocator>::FirstIterator() const
	{
		if (bucketCount == 0)
			return ConstIterator();

		for (uint i = 0; i < bucketCount; ++i)
			if (buckets[i].head != nullptr)
				return ConstIterator(buckets[i].head, this);		
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Set<Value, Hasher, Allocator>::Iterator Set<Value, Hasher, Allocator>::LastIterator()
	{
		if (bucketCount == 0)
			return Iterator();

		for (uint i = bucketCount - 1; i >= 0; --i)
			if (buckets[i].head != nullptr)
				return Iterator(buckets[i].tail, this);
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Set<Value, Hasher, Allocator>::ConstIterator Set<Value, Hasher, Allocator>::LastIterator() const
	{
		if (bucketCount == 0)
			return ConstIterator();

		for (uint i = bucketCount - 1; i >= 0; --i)
			if (buckets[i].head != nullptr)
				return ConstIterator(buckets[i].tail, this);
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Set<Value, Hasher, Allocator>::Iterator Set<Value, Hasher, Allocator>::AheadIterator()
	{
		return Iterator();
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Set<Value, Hasher, Allocator>::ConstIterator Set<Value, Hasher, Allocator>::AheadIterator() const
	{
		return ConstIterator();
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Set<Value, Hasher, Allocator>::Iterator Set<Value, Hasher, Allocator>::BehindIterator()
	{
		return Iterator();
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Set<Value, Hasher, Allocator>::ConstIterator Set<Value, Hasher, Allocator>::BehindIterator() const
	{
		return ConstIterator();
	}	
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Set<Value, Hasher, Allocator>::operator Array<Value>() const
	{
		Array<Value> out { count };
		
		auto src = FirstIterator();
		auto dst = out.FirstIterator();
		const auto end = BehindIterator();

		for (; src != end; ++src, ++dst)
			*dst = *src;

		return out;
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Set<Value, Hasher, Allocator>& Set<Value, Hasher, Allocator>::operator=(const Set& other)
	{
		Clear();

		CopyUnsafe(other);

		return *this;
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Set<Value, Hasher, Allocator>& Set<Value, Hasher, Allocator>::operator=(Set&& other) noexcept
	{
		Clear();

		bucketCount = other.bucketCount;
		buckets = other.buckets;
		count = other.count;
		other.bucketCount = 0;
		other.buckets = nullptr;
		other.count = 0;
		return *this;
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Set<Value, Hasher, Allocator>::Node::Node(Node* prev, Node* next, uint hash, Value&& value)
		: prev(prev), next(next), hash(hash), value(std::move(value))
	{
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline void Set<Value, Hasher, Allocator>::CopyUnsafe(const Set& other)
	{
		bucketCount = other.bucketCount;
		count = other.count;

		buckets = (Bucket*)Memory::Allocate(bucketCount * sizeof(Bucket));

		Bucket* begin = buckets;
		Bucket* end = buckets + bucketCount;
		for (Bucket* itDst = begin, *itSrc = other.buckets; itDst != end; ++itDst, ++itSrc)
		{
			if (itSrc->head == nullptr)
			{
				itDst->head = nullptr;
				continue;
			}

			itDst->head = (Node*)Memory::Allocate(sizeof(Node));

			Node* nodeDst = itDst->head;
			Node* nodeSrc = itSrc->head;

			std::construct_at(nodeDst, nullptr, nullptr, nodeSrc->hash, Value(nodeSrc->value));

			while (nodeSrc->next != nullptr)
			{
				nodeSrc = nodeSrc->next;

				nodeDst->next = (Node*)Memory::Allocate(sizeof(Node));
				std::construct_at(nodeDst->next, nodeDst, nullptr, nodeSrc->hash, Value(nodeSrc->value));

				Node* prev = nodeDst;
				nodeDst = nodeDst->next;

				nodeDst->prev = prev;
			}

			itDst->tail = nodeDst;
		}
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline uint Set<Value, Hasher, Allocator>::Hash(const Value& value)
	{
		return Hasher::Compute(value);
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Set<Value, Hasher, Allocator>::Bucket* Set<Value, Hasher, Allocator>::GetBucketFromHash(uint hash) const
	{
		if (bucketCount == 0)
			return nullptr;

		return GetBucketFromHashModUnsafe(hash % bucketCount);
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Set<Value, Hasher, Allocator>::Bucket* Set<Value, Hasher, Allocator>::GetBucketFromHashModUnsafe(uint hashMod) const
	{
		return &buckets[hashMod];
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Set<Value, Hasher, Allocator>::Iterator Set<Value, Hasher, Allocator>::FindWithHint(const Value& value, uint hash)
	{
		auto* bucket = GetBucketFromHash(hash);

		if (bucket == nullptr) //Not a error. If we were calling Find on an empty set it shouldn't return an error
			return Iterator();

		return FindWithHintUnsafe(value, bucket);
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Set<Value, Hasher, Allocator>::ConstIterator Set<Value, Hasher, Allocator>::FindWithHint(const Value& value, uint hash) const
	{
		auto* bucket = GetBucketFromHash(hash);

		if (bucket == nullptr) //Not a error. If we were calling Find on an empty set it shouldn't return an error
			return ConstIterator();

		return FindWithHintUnsafe(value, bucket);
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Set<Value, Hasher, Allocator>::Iterator Set<Value, Hasher, Allocator>::FindWithHintUnsafe(const Value& value, Bucket* bucket)
	{
		Node* node = bucket->head;

		if (node == nullptr)
			return Iterator();

		while (node->value != value)
		{
			node = node->next;

			if (node == nullptr)
				return Iterator();
		}

		return Iterator(node, this);
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Set<Value, Hasher, Allocator>::ConstIterator Set<Value, Hasher, Allocator>::FindWithHintUnsafe(const Value& value, Bucket* bucket) const
	{
		Node* node = bucket->head;

		if (node == nullptr)
			return ConstIterator();

		while (node->value != value)
		{
			node = node->next;

			if (node == nullptr)
				return ConstIterator();
		}

		return ConstIterator(node, this);
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Set<Value, Hasher, Allocator>::InsertResult Set<Value, Hasher, Allocator>::InsertWithHint(const Value& value, uint hash)
	{
		if (count == 0)
			AllocateEmptyUnsafe();

		Bucket* bucket = GetBucketFromHash(hash);

		if (bucket == nullptr)
		{

			Debug::Logger::LogFatal("Blaze Engine", "Trying to insert a set element but there are no buckets");
			return { Iterator(), false };
		}

		return InsertWithHintUnsafe(value, bucket, hash);
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Set<Value, Hasher, Allocator>::InsertResult Set<Value, Hasher, Allocator>::InsertWithHintUnsafe(const Value& value, Bucket* bucket, uint hash)
	{
		if (bucket->head == nullptr)
		{
			bucket->head = (Node*)allocator.Allocate(sizeof(Node));
			bucket->tail = bucket->head;
			Node* node = bucket->head;

			std::construct_at(node, nullptr, nullptr, hash, Value(value));
			++count;

			return { Iterator(node, this), true };
		}

		Node* node = bucket->head;

		if (node->value == value)
			return { Iterator(node, this), false };

		while (node->next != nullptr)
		{
			node = node->next;

			if (node->value == value)
				return { Iterator(node, this), false };
		}

		node->next = (Node*)allocator.Allocate(sizeof(Node));

		Node* prev = node;
		node = node->next;

		std::construct_at(node, prev, nullptr, hash, Value(value));
		++count;

		CheckForRehash();

		return { Iterator(node, this), true };
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline void Set<Value, Hasher, Allocator>::EraseNodeUnsafe(Bucket* bucket, Node* node)
	{
		RemoveNodeFromSpotUnsafe(node, bucket);

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK				
		if (node->iteratorCount)
			Debug::Logger::LogWarning("Blaze Engine", "Erasing a set node while a iterator is referencing it");
#endif

		std::destroy_at(node);
		allocator.Free(node);
		--count;		
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Result Set<Value, Hasher, Allocator>::CheckForRehash()
	{
		static constexpr float max_load_factor = 1.0f;

		float loadFactor = (float)count / bucketCount;

		if (loadFactor > max_load_factor)
			return Rehash(bucketCount * 2);
		else if (loadFactor * 4 < max_load_factor)
		{
			if (bucketCount == 8)
				return Result();

			uint newBucketCount = (bucketCount + 1) / 2;

			if (newBucketCount < 8)
				newBucketCount = 8;

			return Rehash(newBucketCount);
		}

		return Result();
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Result Set<Value, Hasher, Allocator>::Rehash(uint newBucketCount)
	{
		Bucket* old = buckets;
		buckets = (Bucket*)Memory::Allocate(sizeof(Bucket) * newBucketCount);
		memset(buckets, 0, sizeof(Bucket) * newBucketCount);

		Bucket* begin = old;
		Bucket* end = old + bucketCount;

		Result result;
		for (Bucket* it = begin; it != end; ++it)
			result += RehashBucketUnsafe(it, buckets, newBucketCount);

		Memory::Free(old);
		bucketCount = newBucketCount;

		return result;
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline void Set<Value, Hasher, Allocator>::AllocateEmptyUnsafe()
	{
		bucketCount = 8;
		buckets = (Bucket*)Memory::Allocate(sizeof(Bucket) * bucketCount);
		memset(buckets, 0, sizeof(Bucket) * bucketCount);
		count = 0;
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline Result Set<Value, Hasher, Allocator>::RehashBucketUnsafe(Bucket* bucket, Bucket* bucketArray, uint bucketArraySize)
	{
		Node* node = bucket->head;

		while (node != nullptr)
		{
			Node* next = node->next;

			uint newHashMod = node->hash % bucketArraySize;
			Bucket* newBucket = &bucketArray[newHashMod];

			MoveNodeToBeginingUnsafe(node, newBucket);

			node = next;
		}

		return Result();
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline void Set<Value, Hasher, Allocator>::RemoveNodeFromSpotUnsafe(Node* node, Bucket* bucket)
	{
		if (node->prev)
		{
			Node* prev = node->prev;

			if (node->next)
			{
				Node* next = node->next;

				prev->next = next;
				next->prev = prev;
			}
			else
			{
				prev->next = nullptr;
				bucket->tail = prev;
			}
		}
		else
		{
			Node* next = node->next;
			
			if (node->next)
			{
				next->prev = nullptr;
				bucket->head = next;
			}
			else
			{
				bucket->head = nullptr;
				bucket->tail = nullptr;
			}

		}
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline void Set<Value, Hasher, Allocator>::MoveNodeUnsafe(Node* node, Bucket* oldBucket, Bucket* newBucket)
	{
		RemoveNodeFromSpotUnsafe(node, oldBucket);
		MoveNodeToBeginingUnsafe(node, newBucket);
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	inline void Set<Value, Hasher, Allocator>::MoveNodeToBeginingUnsafe(Node* node, Bucket* bucket)
	{
		Node* head = bucket->head;
		node->next = head;
		node->prev = nullptr;

		if (head != nullptr)
			head->prev = node;
	}

	template<typename Value, typename Hasher, AllocatorType Allocator>
	Set<Value, Hasher, Allocator>::Iterator begin(Set<Value, Hasher, Allocator>& set)
	{
		return set.FirstIterator();
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	Set<Value, Hasher, Allocator>::ConstIterator begin(const Set<Value, Hasher, Allocator>& set)
	{
		return set.FirstIterator();
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	Set<Value, Hasher, Allocator>::Iterator end(Set<Value, Hasher, Allocator>& set)
	{
		return set.BehindIterator();
	}
	template<typename Value, typename Hasher, AllocatorType Allocator>
	Set<Value, Hasher, Allocator>::ConstIterator end(const Set<Value, Hasher, Allocator>& set)
	{
		return set.BehindIterator();
	}
}