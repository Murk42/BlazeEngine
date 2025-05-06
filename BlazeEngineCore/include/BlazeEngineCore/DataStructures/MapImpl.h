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
			BLAZE_ENGINE_CORE_FATAL("Incrementing a null iterator");
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

		if (bucket == map->buckets + map->bucketCount)
		{
			node = nullptr;
			return *this;
		}

		while (bucket->head == nullptr)
		{
			++bucket;

			if (bucket == map->buckets + map->bucketCount)
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
			BLAZE_ENGINE_CORE_FATAL("Decrementing a null iterator");
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

		if (bucket == map->buckets)
		{
			node = nullptr;
			return *this;
		}

		--bucket;

		while (bucket->tail == nullptr)
		{
			if (bucket == map->buckets)
			{
				node = nullptr;
				return *this;
			}

			--bucket;			
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
			BLAZE_ENGINE_CORE_FATAL("Dereferencing a null iterator");
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
			BLAZE_ENGINE_CORE_FATAL("Dereferencing a null iterator");
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
					BLAZE_ENGINE_CORE_WARNING("Clearing a map while a iterator is referencing it");
#endif
				Node* next = node->next;

				std::destroy_at(node);
				allocator.Free(node);				
				node = next;
			}
		}

		allocator.Free(buckets);

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
	template<typename ... Args> requires std::constructible_from<Value, Args...>
	inline Map<Key, Value, Hasher, Allocator>::InsertResult Map<Key, Value, Hasher, Allocator>::Insert(const Key& key, Args&& ... args)
	{
		return InsertWithHint(key, Hash(key), std::forward<Args>(args)...);
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline void Map<Key, Value, Hasher, Allocator>::Erase(const Key& key)
	{
		Bucket* bucket = GetBucketFromHash(Hash(key));

		if (bucket == nullptr)
		{
			BLAZE_ENGINE_CORE_FATAL("Erasing a map element from an empty map");
			return;
		}			

		auto it = FindWithHintUnsafe(key, bucket);

		if (it.IsNull())
		{
			BLAZE_ENGINE_CORE_FATAL("Erasing a invalid element from a map");
			return;
		}

		EraseNodeUnsafe(bucket, it.node);		
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline void Map<Key, Value, Hasher, Allocator>::Erase(const Iterator& iterator)
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (iterator.IsNull())
		{
			BLAZE_ENGINE_CORE_ERROR("Trying to erase a map pair with a null iterator");
			return;
		}
#endif

		Bucket* bucket = GetBucketFromHash(iterator.node->hash);

		if (bucket == nullptr)
		{
			BLAZE_ENGINE_CORE_FATAL("Erasing a map element from an empty map");
			return;
		}

		EraseNodeUnsafe(bucket, iterator.node);		
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline uintMem Map<Key, Value, Hasher, Allocator>::Count() const
	{
		return count;
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline Map<Key, Value, Hasher, Allocator>::Iterator Map<Key, Value, Hasher, Allocator>::FirstIterator()
	{
		if (bucketCount != 0)			
			for (uintMem i = 0; i < bucketCount; ++i)
				if (buckets[i].head != nullptr)
					return Iterator(buckets[i].head, this);

		return Iterator();
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
	template<typename ... Args> requires std::constructible_from<Value, Args...>
	inline Map<Key, Value, Hasher, Allocator>::Node::Node(Node* prev, Node* next, uintMem hash, Key&& key, Args&& ... args)
		: prev(prev), next(next), hash(hash), pair(std::move(key), std::forward<Args>(args)...)
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

		buckets = (Bucket*)allocator.Allocate(bucketCount * sizeof(Bucket));

		Bucket* begin = buckets;
		Bucket* end = buckets + bucketCount;
		for (Bucket* itDst = begin, *itSrc = other.buckets; itDst != end; ++itDst, ++itSrc)
		{
			if (itSrc->head == nullptr)
			{
				itDst->head = nullptr;
				continue;
			}

			itDst->head = (Node*)allocator.Allocate(sizeof(Node));

			Node* nodeDst = itDst->head;
			Node* nodeSrc = itSrc->head;

			std::construct_at(nodeDst, nullptr, nullptr, nodeSrc->hash, Key(nodeSrc->pair.key), Value(nodeSrc->pair.value));

			while (nodeSrc->next != nullptr)
			{
				nodeSrc = nodeSrc->next;

				nodeDst->next = (Node*)allocator.Allocate(sizeof(Node));
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
	template<typename ... Args> requires  std::constructible_from<Value, Args...>
	inline Map<Key, Value, Hasher, Allocator>::InsertResult Map<Key, Value, Hasher, Allocator>::InsertWithHint(const Key& key, uintMem hash, Args&& ... args)
	{
		if (count == 0)
			AllocateEmptyUnsafe();

		Bucket* bucket = GetBucketFromHash(hash);

		if (bucket == nullptr)
		{
			BLAZE_ENGINE_CORE_FATAL("Trying to insert a map pair but there are no buckets");
			return { Iterator(), false };
		}

		return InsertWithHintUnsafe(key, bucket, hash, std::forward<Args>(args)...);
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	template<typename ... Args> requires std::constructible_from<Value, Args...>
	inline Map<Key, Value, Hasher, Allocator>::InsertResult Map<Key, Value, Hasher, Allocator>::InsertWithHintUnsafe(const Key& key, Bucket* bucket, uintMem hash, Args&& ... args)
	{
		Node* node = nullptr;
		Node* prev = nullptr;
		Node* next = nullptr;

		if (bucket->head == nullptr)
		{
			bucket->head = (Node*)allocator.Allocate(sizeof(Node));
			bucket->tail = bucket->head;

			++count;

			node = bucket->head;			
		}
		else
		{
			node = bucket->head;

			bool found = false;

			do 
			{		
				//if constexpr (!Replace)
				{
					if (node->pair.key == key)
						return { Iterator(node, this), false };
				}
				//else
				//{
				//	//TODO implement assignment
				//}				

				if (node->next == nullptr)
					break;

				node = node->next;
			} while (true);

			if (!found)
			{
				node->next = (Node*)allocator.Allocate(sizeof(Node));
				bucket->tail = node->next;

				++count;

				prev = node;
				node = node->next;
			}
		}		

		std::construct_at(node, prev, next, hash, Key(key), std::forward<Args>(args)...);		

		CheckForRehash();

		return { Iterator(node, this), true };
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline void Map<Key, Value, Hasher, Allocator>::EraseNodeUnsafe(Bucket* bucket, Node* node)
	{
		RemoveNodeFromSpotUnsafe(node, bucket);

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK				
		if (node->iteratorCount)
			BLAZE_ENGINE_CORE_WARNING("Erasing a map node while a iterator is referencing it");
#endif

		std::destroy_at(node);
		allocator.Free(node);
		--count;		
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline void Map<Key, Value, Hasher, Allocator>::CheckForRehash()
	{
		static constexpr float max_load_factor = 1.0f;

		float loadFactor = (float)count / bucketCount;

		if (loadFactor > max_load_factor)
			return Rehash(bucketCount * 2);
		else if (loadFactor * 4 < max_load_factor)
		{
			if (bucketCount == 8)
				return;

			uintMem newBucketCount = (bucketCount + 1) / 2;

			if (newBucketCount < 8)
				newBucketCount = 8;

			Rehash(newBucketCount);
		}
		
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline void Map<Key, Value, Hasher, Allocator>::Rehash(uintMem newBucketCount)
	{
		Bucket* old = buckets;
		buckets = (Bucket*)allocator.Allocate(sizeof(Bucket) * newBucketCount);
		memset(buckets, 0, sizeof(Bucket) * newBucketCount);

		Bucket* begin = old;
		Bucket* end = old + bucketCount;
		
		for (Bucket* it = begin; it != end; ++it)
			RehashBucketUnsafe(it, buckets, newBucketCount);

		allocator.Free(old);
		bucketCount = newBucketCount;
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline void Map<Key, Value, Hasher, Allocator>::AllocateEmptyUnsafe()
	{
		bucketCount = 8;
		buckets = (Bucket*)allocator.Allocate(sizeof(Bucket) * bucketCount);
		memset(buckets, 0, sizeof(Bucket) * bucketCount);
		count = 0;
	}
	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	inline void Map<Key, Value, Hasher, Allocator>::RehashBucketUnsafe(Bucket* bucket, Bucket* bucketArray, uintMem bucketArraySize)
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