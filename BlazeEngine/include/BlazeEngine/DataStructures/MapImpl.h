#include "Map.h"
#pragma once

namespace Blaze
{
	template<typename Map>
	inline MapIterator<Map>::MapIterator()
		: map(nullptr), node(nullptr)
	{
	}
	template<typename Map>
	template<IsConvertibleToMapIterator<MapIterator<Map>> T>
	inline MapIterator<Map>::MapIterator(const T& other)
		: map(other.map), node(other.node)
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (node != nullptr)
			++node->iteratorCount;
#endif
	}
	template<typename Map>
	inline bool MapIterator<Map>::IsNull() const
	{
		return node == nullptr;
	}
	template<typename Map>
	inline MapIterator<Map>& MapIterator<Map>::operator++()
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

		auto* bucket = map->GetBucketFromHash(node->hash);

		++bucket;
		while (bucket->head == nullptr)
		{
			++bucket;

			if (bucket - map->buckets == map->bucketCount)
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
	template<typename Map>
	inline MapIterator<Map> MapIterator<Map>::operator++(int)
	{
		auto copy = *this;
		++copy;
		return copy;
	}
	template<typename Map>
	inline MapIterator<Map>& MapIterator<Map>::operator--()
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

		auto* bucket = map->GetBucketFromHash(node->hash);

		--bucket;
		while (bucket->tail == nullptr)
		{
			--bucket;

			if (bucket == map->buckets)
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
	template<typename Map>
	inline MapIterator<Map> MapIterator<Map>::operator--(int)
	{
		auto copy = *this;
		--copy;
		return copy;
	}
	template<typename Map>
	inline MapIterator<Map>::MapPairType& MapIterator<Map>::operator*() const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (node == nullptr)
		{
			Debug::Logger::LogFatal("Blaze Engine", "Dereferencing a null iterator");
			return *(MapPairType*)nullptr;
		}
#endif

		return node->pair;
	}
	template<typename Map>
	inline MapIterator<Map>::MapPairType* MapIterator<Map>::operator->() const
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (node == nullptr)
		{
			Debug::Logger::LogFatal("Blaze Engine", "Dereferencing a null iterator");
			return nullptr;
		}
#endif

		return (MapPairType*)&node->pair;
	}
	template<typename Map>
	template<IsComparableToMapIterator<MapIterator<Map>> T>
	inline bool MapIterator<Map>::operator==(const T& other) const
	{
		return node == other.node;
	}
	template<typename Map>
	template<IsComparableToMapIterator<MapIterator<Map>> T>
	inline bool MapIterator<Map>::operator!=(const T& other) const
	{
		return node != other.node;
	}
	template<typename Map>
	template<IsConvertibleToMapIterator<MapIterator<Map>> T>
	inline MapIterator<Map>& MapIterator<Map>::operator=(const T& other)
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (node != nullptr)
			--node->iteratorCount;
#endif
		map = other.map;
		node = other.node;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (node != nullptr)
			++node->iteratorCount;
#endif
	}
	template<typename Map>
	inline MapIterator<Map>::MapIterator(MapNode* node, Map* map)
		: map(map), node(node)
	{

	}

	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Map<Key, Value, Hasher, Allocator>::Map()
		: buckets(nullptr), bucketCount(0), count(0)
	{		
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Map<Key, Value, Hasher, Allocator>::Map(const Map& other)
		: buckets(nullptr), bucketCount(0), count(0)
	{
		CopyUnsafe(other);
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Map<Key, Value, Hasher, Allocator>::Map(Map&& other) noexcept
		: bucketCount(other.bucketCount), buckets(other.buckets), count(other.count)
	{
		other.bucketCount = 0;
		other.buckets = nullptr;
		other.count = 0;
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Map<Key, Value, Hasher, Allocator>::Map(const std::initializer_list<MapPairType>& other)
		: buckets(nullptr), bucketCount(0), count(0)
	{
		for (const auto& el : other)
			Insert(el.key, el.value);
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Map<Key, Value, Hasher, Allocator>::~Map()
	{
		Clear();
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline void Map<Key, Value, Hasher, Allocator>::Clear()
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
					Debug::Logger::LogWarning("Blaze Engine", "Clearing a map while a iterator is referencing it");
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
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Map<Key, Value, Hasher, Allocator>::Iterator Map<Key, Value, Hasher, Allocator>::Find(const Key& key)
	{
		return FindWithHint(key, Hash(key));
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Map<Key, Value, Hasher, Allocator>::ConstIterator Map<Key, Value, Hasher, Allocator>::Find(const Key& key) const
	{
		return ConstIterator(FindWithHint(key, Hash(key)));
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline bool  Map<Key, Value, Hasher, Allocator>::Contains(const Key& key) const
	{
		uintMem hash = Hash(key);
		auto* bucket = GetBucketFromHash(hash);

		if (bucket == nullptr) //Not a error. If we were calling Contains on an empty set it shouldn't return an error
			return false;

		Node* node = bucket->head;

		if (node == nullptr)
			return false;

		while (node->pair.key != key)
		{
			node = node->next;

			if (node == nullptr)
				return false;
		}

		return true;
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Map<Key, Value, Hasher, Allocator>::InsertResult Map<Key, Value, Hasher, Allocator>::Insert(const Key& key, const Value& value)
	{
		return InsertWithHint(key, value, Hash(key));
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline bool Map<Key, Value, Hasher, Allocator>::Erase(const Key& key)
	{
		Bucket* bucket = GetBucketFromHash(Hash(key));

		if (bucket == nullptr)
			return false;

		auto it = FindWithHintUnsafe(key, bucket);

		if (it.IsNull())
			return false;

		EraseNodeUnsafe(bucket, it.node);

		return true;
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline bool Map<Key, Value, Hasher, Allocator>::Erase(const Iterator& iterator)
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (iterator.IsNull())
			return BLAZE_ERROR_RESULT("Blaze Engine", "Trying to erase a map pair with a null iterator");
#endif

		Bucket* bucket = GetBucketFromHash(iterator.node->hash);

		if (bucket == nullptr)
			return false;

		EraseNodeUnsafe(bucket, iterator.node);
		
		return true;
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline uintMem Map<Key, Value, Hasher, Allocator>::Count() const
	{
		return count;
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Map<Key, Value, Hasher, Allocator>::Iterator Map<Key, Value, Hasher, Allocator>::FirstIterator()
	{
		if (bucketCount == 0)
			return Iterator();

		for (uintMem i = 0; i < bucketCount; ++i)
			if (buckets[i].head != nullptr)
				return Iterator(buckets[i].head, this);
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Map<Key, Value, Hasher, Allocator>::ConstIterator Map<Key, Value, Hasher, Allocator>::FirstIterator() const
	{
		for (uintMem i = 0; i < bucketCount; ++i)
			if (buckets[i].head != nullptr)
				return ConstIterator(buckets[i].head, this);

		return ConstIterator();
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Map<Key, Value, Hasher, Allocator>::Iterator Map<Key, Value, Hasher, Allocator>::LastIterator()
	{
		if (bucketCount == 0)
			return Iterator();

		for (uintMem i = bucketCount - 1; i >= 0; --i)
			if (buckets[i].head != nullptr)
				return Iterator(buckets[i].tail, this);
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Map<Key, Value, Hasher, Allocator>::ConstIterator Map<Key, Value, Hasher, Allocator>::LastIterator() const
	{
		if (bucketCount == 0)
			return ConstIterator();

		for (uintMem i = bucketCount - 1; i >= 0; --i)
			if (buckets[i].head != nullptr)
				return ConstIterator(buckets[i].tail, this);
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Map<Key, Value, Hasher, Allocator>::Iterator Map<Key, Value, Hasher, Allocator>::AheadIterator()
	{
		return Iterator();
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Map<Key, Value, Hasher, Allocator>::ConstIterator Map<Key, Value, Hasher, Allocator>::AheadIterator() const
	{
		return ConstIterator();
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Map<Key, Value, Hasher, Allocator>::Iterator Map<Key, Value, Hasher, Allocator>::BehindIterator()
	{
		return Iterator();
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Map<Key, Value, Hasher, Allocator>::ConstIterator Map<Key, Value, Hasher, Allocator>::BehindIterator() const
	{
		return ConstIterator();
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Map<Key, Value, Hasher, Allocator>& Map<Key, Value, Hasher, Allocator>::operator=(const Map& other)
	{
		Clear();

		CopyUnsafe(other);

		return *this;
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Map<Key, Value, Hasher, Allocator>& Map<Key, Value, Hasher, Allocator>::operator=(Map&& other) noexcept
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
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Map<Key, Value, Hasher, Allocator>::Node::Node(Node* prev, Node* next, uintMem hash, Key&& key, Value&& value)
		: prev(prev), next(next), hash(hash), pair(std::move(key), std::move(value))
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		, iteratorCount(0)
#endif		
	{
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline void Map<Key, Value, Hasher, Allocator>::CopyUnsafe(const Map& other)
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

			std::construct_at(nodeDst, nullptr, nullptr, nodeSrc->hash, Key(nodeSrc->pair.key), Value(nodeSrc->pair.value));

			while (nodeSrc->next != nullptr)
			{
				nodeSrc = nodeSrc->next;

				nodeDst->next = (Node*)Memory::Allocate(sizeof(Node));
				std::construct_at(nodeDst->next, nodeDst, nullptr, nodeSrc->hash, Key(nodeSrc->pair.key), Value(nodeSrc->pair.value));

				Node* prev = nodeDst;
				nodeDst = nodeDst->next;

				nodeDst->prev = prev;
			}

			itDst->tail = nodeDst;
		}
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline uintMem Map<Key, Value, Hasher, Allocator>::Hash(const Key& key)
	{
		return Hasher::Compute(key);
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Map<Key, Value, Hasher, Allocator>::Bucket* Map<Key, Value, Hasher, Allocator>::GetBucketFromHash(uintMem hash) const
	{
		if (bucketCount == 0)
			return nullptr;

		return GetBucketFromHashModUnsafe(hash % bucketCount);
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Map<Key, Value, Hasher, Allocator>::Bucket* Map<Key, Value, Hasher, Allocator>::GetBucketFromHashModUnsafe(uintMem hashMod) const
	{
		return &buckets[hashMod];
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Map<Key, Value, Hasher, Allocator>::Iterator Map<Key, Value, Hasher, Allocator>::FindWithHint(const Key& key, uintMem hash)
	{
		auto* bucket = GetBucketFromHash(hash);

		if (bucket == nullptr) //Not a error. If we were calling Find on an empty map it shouldn't return an error
			return Iterator();

		return FindWithHintUnsafe(key, bucket);
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Map<Key, Value, Hasher, Allocator>::ConstIterator Map<Key, Value, Hasher, Allocator>::FindWithHint(const Key& key, uintMem hash) const
	{
		auto* bucket = GetBucketFromHash(hash);

		if (bucket == nullptr) //Not a error. If we were calling Find on an empty map it shouldn't return an error
			return ConstIterator();

		return FindWithHintUnsafe(key, bucket);
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Map<Key, Value, Hasher, Allocator>::Iterator Map<Key, Value, Hasher, Allocator>::FindWithHintUnsafe(const Key& key, Bucket* bucket)
	{
		Node* node = bucket->head;

		if (node == nullptr)
			return Iterator();

		while (node->pair.key != key)
		{
			node = node->next;

			if (node == nullptr)
				return Iterator();
		}

		return Iterator(node, this);
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Map<Key, Value, Hasher, Allocator>::ConstIterator Map<Key, Value, Hasher, Allocator>::FindWithHintUnsafe(const Key& key, Bucket* bucket) const
	{
		Node* node = bucket->head;

		if (node == nullptr)
			return ConstIterator();

		while (node->pair.key != key)
		{
			node = node->next;

			if (node == nullptr)
				return ConstIterator();
		}

		return ConstIterator(node, this);
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Map<Key, Value, Hasher, Allocator>::InsertResult Map<Key, Value, Hasher, Allocator>::InsertWithHint(const Key& key, const Value& value, uintMem hash)
	{
		if (count == 0)
			AllocateEmptyUnsafe();

		Bucket* bucket = GetBucketFromHash(hash);

		if (bucket == nullptr)
		{
			Debug::Logger::LogFatal("Blaze Engine", "Trying to insert a map pair but there are no buckets");
			return { Iterator(), false };
		}

		return InsertWithHintUnsafe(key, value, bucket, hash);
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Map<Key, Value, Hasher, Allocator>::InsertResult Map<Key, Value, Hasher, Allocator>::InsertWithHintUnsafe(const Key& key, const Value& value, Bucket* bucket, uintMem hash)
	{
		if (bucket->head == nullptr)
		{
			bucket->head = (Node*)allocator.Allocate(sizeof(Node));
			bucket->tail = bucket->head;
			Node* node = bucket->head;

			std::construct_at(node, nullptr, nullptr, hash, Key(key), Value(value));
			++count;

			return { Iterator(node, this), true };
		}

		Node* node = bucket->head;

		if (node->pair.key == key)
			return { Iterator(node, this), false };

		while (node->next != nullptr)
		{
			node = node->next;

			if (node->pair.key == key)
				return { Iterator(node, this), false };
		}

		node->next = (Node*)allocator.Allocate(sizeof(Node));

		Node* prev = node;
		node = node->next;

		std::construct_at(node, prev, nullptr, hash, Key(key), Value(value));
		++count;

		CheckForRehash();

		return { Iterator(node, this), true };
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline void Map<Key, Value, Hasher, Allocator>::EraseNodeUnsafe(Bucket* bucket, Node* node)
	{
		RemoveNodeFromSpotUnsafe(node, bucket);

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK				
		if (node->iteratorCount)
			Debug::Logger::LogWarning("Blaze Engine", "Erasing a map node while a iterator is referencing it");
#endif

		std::destroy_at(node);
		allocator.Free(node);
		--count;		
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Result Map<Key, Value, Hasher, Allocator>::CheckForRehash()
	{
		static constexpr float max_load_factor = 1.0f;

		float loadFactor = (float)count / bucketCount;

		if (loadFactor > max_load_factor)
			return Rehash(bucketCount * 2);
		else if (loadFactor * 4 < max_load_factor)
		{
			if (bucketCount == 8)
				return Result();

			uintMem newBucketCount = (bucketCount + 1) / 2;

			if (newBucketCount < 8)
				newBucketCount = 8;

			return Rehash(newBucketCount);
		}

		return Result();
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Result Map<Key, Value, Hasher, Allocator>::Rehash(uintMem newBucketCount)
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
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline void Map<Key, Value, Hasher, Allocator>::AllocateEmptyUnsafe()
	{
		bucketCount = 8;
		buckets = (Bucket*)Memory::Allocate(sizeof(Bucket) * bucketCount);
		memset(buckets, 0, sizeof(Bucket) * bucketCount);
		count = 0;
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Result Map<Key, Value, Hasher, Allocator>::RehashBucketUnsafe(Bucket* bucket, Bucket* bucketArray, uintMem bucketArraySize)
	{
		Node* node = bucket->head;

		while (node != nullptr)
		{
			Node* next = node->next;

			uintMem newHashMod = node->hash % bucketArraySize;
			Bucket* newBucket = &bucketArray[newHashMod];

			MoveNodeToBeginingUnsafe(node, newBucket);

			node = next;
		}

		return Result();
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline void Map<Key, Value, Hasher, Allocator>::RemoveNodeFromSpotUnsafe(Node* node, Bucket* bucket)
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
	//template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	//inline void Map<Key, Value, Hasher, Allocator>::MoveNodeUnsafe(Node* node, Bucket* oldBucket, Bucket* newBucket)
	//{
	//	RemoveNodeFromSpotUnsafe(node, oldBucket);
	//	MoveNodeToBeginingUnsafe(node, newBucket);
	//}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline void Map<Key, Value, Hasher, Allocator>::MoveNodeToBeginingUnsafe(Node* node, Bucket* bucket)
	{
		Node* head = bucket->head;
		node->next = head;
		node->prev = nullptr;


		if (head != nullptr)
			head->prev = node;
		else
			bucket->tail = node;

		bucket->head = node;
	}

	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	Map<Key, Value, Hasher, Allocator>::Iterator begin(Map<Key, Value, Hasher, Allocator>& map)
	{
		return map.FirstIterator();
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	Map<Key, Value, Hasher, Allocator>::ConstIterator begin(const Map<Key, Value, Hasher, Allocator>& map)
	{
		return map.FirstIterator();
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	Map<Key, Value, Hasher, Allocator>::Iterator end(Map<Key, Value, Hasher, Allocator>& map)
	{
		return map.BehindIterator();
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	Map<Key, Value, Hasher, Allocator>::ConstIterator end(const Map<Key, Value, Hasher, Allocator>& map)
	{
		return map.BehindIterator();
	}
}