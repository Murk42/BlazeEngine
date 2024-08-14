#pragma once
#include "VirtualMap.h"
#include "VirtualList.h"

namespace Blaze
{
	template<typename VirtualMap>
	inline VirtualMapIterator<VirtualMap>::VirtualMapIterator()
		: map(nullptr), nodeHeader(nullptr)
	{
	}
	template<typename VirtualMap>
	template<IsConvertibleToVirtualMapIterator<VirtualMapIterator<VirtualMap>> T>
	inline VirtualMapIterator<VirtualMap>::VirtualMapIterator(const T& other)
		: map(other.map), nodeHeader(other.nodeHeader)
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (nodeHeader != nullptr)
			++nodeHeader->iteratorCount;
#endif
	}	
	template<typename VirtualMap>
	inline bool VirtualMapIterator<VirtualMap>::IsNull() const
	{
		return nodeHeader == nullptr && map == nullptr;
	}
	template<typename VirtualMap>
	inline VirtualMapIterator<VirtualMap>& VirtualMapIterator<VirtualMap>::operator++()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK				
		if (nodeHeader == nullptr)
			Debug::Logger::LogFatal("Blaze Engine", "Incrementing a null iterator");
#endif		

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (nodeHeader != nullptr)
			--node->iteratorCount;
#endif

		if (nodeHeader->next != nullptr)
		{
			nodeHeader = nodeHeader->next;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
			--nodeHeader->iteratorCount;
#endif

			return *this;
		}

		auto* bucket = map->GetBucketFromHash(nodeHeader->hash);

		++bucket;

		if (bucket == map->buckets + map->bucketCount)
		{
			nodeHeader = nullptr;
			return *this;
		}

		while (bucket->head == nullptr)
		{
			++bucket;

			if (bucket == map->buckets + map->bucketCount)
			{
				nodeHeader = nullptr;
				return *this;
			}
		}

		nodeHeader = bucket->head;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		--nodeHeader->iteratorCount;
#endif
		return *this;
	}
	template<typename VirtualMap>
	inline VirtualMapIterator<VirtualMap> VirtualMapIterator<VirtualMap>::operator++(int)
	{
		auto copy = *this;
		++copy;
		return copy;
	}
	template<typename VirtualMap>
	inline VirtualMapIterator<VirtualMap>& VirtualMapIterator<VirtualMap>::operator--()
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK			
		if (nodeHeader == nullptr)
			Debug::Logger::LogFatal("Blaze Engine", "Decrementing a null iterator");
#endif			

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		--nodeHeader->iteratorCount;
#endif

		if (nodeHeader->prev != nullptr)
		{
			nodeHeader = nodeHeader->prev;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
			++nodeHeader->iteratorCount;
#endif

			return *this;
		}

		auto* bucket = map->GetBucketFromHash(nodeHeader->hash);

		if (bucket == map->buckets)
		{
			nodeHeader = nullptr;
			return *this;
		}

		--bucket;

		while (bucket->tail == nullptr)
		{
			if (bucket == map->buckets)
			{
				nodeHeader = nullptr;
				return *this;
			}

			--bucket;			
		}

		nodeHeader = bucket->tail;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		++nodeHeader->iteratorCount;
#endif

		return *this;
	}
	template<typename VirtualMap>
	inline VirtualMapIterator<VirtualMap> VirtualMapIterator<VirtualMap>::operator--(int)
	{
		auto copy = *this;
		--copy;
		return copy;
	}	
	template<typename VirtualMap>
	template<typename Value> requires InsertableToVirtualMap<Value, typename VirtualMap::template ValueBaseType>
	inline typename VirtualMapIterator<VirtualMap>::template MatchMapConst<Value>* VirtualMapIterator<VirtualMap>::GetValue() const
	{		
		if (nodeHeader == nullptr)
			return nullptr;			

		auto node = dynamic_cast<typename VirtualMap::template Node<Value>*>(nodeHeader);
		
		if (node == nullptr)
			return nullptr;

		return &node->value;
	}
	template<typename VirtualMap>
	inline const typename VirtualMap::template KeyType* VirtualMapIterator<VirtualMap>::GetKey() const
	{
		if (nodeHeader == nullptr)
			return nullptr;			
		return &nodeHeader->key;
	}
	template<typename VirtualMap>
	template<IsComparableToVirtualMapIterator<VirtualMapIterator<VirtualMap>> T>
	inline bool VirtualMapIterator<VirtualMap>::operator==(const T& other) const
	{
		return nodeHeader == other.nodeHeader;
	}
	template<typename VirtualMap>
	template<IsComparableToVirtualMapIterator<VirtualMapIterator<VirtualMap>> T>
	inline bool VirtualMapIterator<VirtualMap>::operator!=(const T& other) const
	{
		return nodeHeader != other.nodeHeader;
	}
	template<typename VirtualMap>
	template<IsConvertibleToVirtualMapIterator<VirtualMapIterator<VirtualMap>> T>
	inline VirtualMapIterator<VirtualMap>& VirtualMapIterator<VirtualMap>::operator=(const T& other)
	{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (nodeHeader != nullptr)
			--nodeHeader->iteratorCount;
#endif
		map = other.map;
		nodeHeader = other.nodeHeader;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		if (nodeHeader != nullptr)
			++nodeHeader->iteratorCount;
#endif
	}
	template<typename VirtualMap>
	inline VirtualMapIterator<VirtualMap>::VirtualMapIterator(VirtualMapNodeHeader* nodeHeader, VirtualMap* map)
		: map(map), nodeHeader(nodeHeader)
	{

	}



	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, ValueBase, Hasher, Allocator>::VirtualMap()
		: buckets(nullptr), bucketCount(0), count(0)
	{
	}	
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, ValueBase, Hasher, Allocator>::VirtualMap(VirtualMap&& other) noexcept
		: bucketCount(other.bucketCount), buckets(other.buckets), count(other.count)
	{
		other.bucketCount = 0;
		other.buckets = nullptr;
		other.count = 0;
	}	
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, ValueBase, Hasher, Allocator>::~VirtualMap()
	{
		Clear();
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline void VirtualMap<Key, ValueBase, Hasher, Allocator>::Clear()
	{
		Bucket* begin = buckets;
		Bucket* end = buckets + bucketCount;
		for (Bucket* it = begin; it != end; ++it)
		{
			NodeHeader* nodeHeader = it->head;
			while (nodeHeader != nullptr)
			{
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK				
				if (nodeHeader->iteratorCount)
					Debug::Logger::LogWarning("Blaze Engine", "Clearing a map while a iterator is referencing it");
#endif
				NodeHeader* next = nodeHeader->next;

				std::destroy_at(nodeHeader);
				allocator.Free(nodeHeader);
				nodeHeader = next;
			}
		}

		allocator.Free(buckets);

		buckets = nullptr;
		bucketCount = 0;
		count = 0;
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline bool VirtualMap<Key, ValueBase, Hasher, Allocator>::Empty() const
	{
		return count == 0;
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, ValueBase, Hasher, Allocator>::Iterator VirtualMap<Key, ValueBase, Hasher, Allocator>::Find(const Key& key)
	{
		return FindWithHint(key, Hash(key));
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, ValueBase, Hasher, Allocator>::ConstIterator VirtualMap<Key, ValueBase, Hasher, Allocator>::Find(const Key& key) const
	{
		return ConstIterator(FindWithHint(key, Hash(key)));
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline bool  VirtualMap<Key, ValueBase, Hasher, Allocator>::Contains(const Key& key) const
	{
		uintMem hash = Hash(key);
		auto* bucket = GetBucketFromHash(hash);

		if (bucket == nullptr) //Not a error. If we were calling Contains on an empty set it shouldn't return an error
			return false;

		NodeHeader* node = bucket->head;

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
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	template<typename Value, bool Replace, typename _Key, typename ... Args> requires InsertableToVirtualMap<Value, ValueBase> && std::constructible_from<Key, _Key> && std::constructible_from<Value, Args...>
	inline VirtualMap<Key, ValueBase, Hasher, Allocator>::InsertResult VirtualMap<Key, ValueBase, Hasher, Allocator>::Insert(_Key&& key, Args&& ... args)
	{
		return InsertWithHint<Replace, _Key, Value>(std::forward<_Key>(key), Hash(key), std::forward<Args>(args)...);
	}	
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline bool VirtualMap<Key, ValueBase, Hasher, Allocator>::Erase(const Key& key)
	{
		Bucket* bucket = GetBucketFromHash(Hash(key));

		if (bucket == nullptr)
			return false;

		auto it = FindWithHintUnsafe(key, bucket);

		if (it.IsNull())
			return false;

		EraseNodeUnsafe(bucket, it.nodeHeader);

		return true;
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline bool VirtualMap<Key, ValueBase, Hasher, Allocator>::Erase(const Iterator& iterator)
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (iterator.IsNull())
			return BLAZE_ERROR_RESULT("Blaze Engine", "Trying to erase a map pair with a null iterator");
#endif

		Bucket* bucket = GetBucketFromHash(iterator.node->hash);

		if (bucket == nullptr)
			return false;

		EraseNodeUnsafe(bucket, iterator.nodeHeader);

		return true;
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline uintMem VirtualMap<Key, ValueBase, Hasher, Allocator>::Count() const
	{
		return count;
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, ValueBase, Hasher, Allocator>::Iterator VirtualMap<Key, ValueBase, Hasher, Allocator>::FirstIterator()
	{
		if (bucketCount == 0)
			return Iterator();

		for (uintMem i = 0; i < bucketCount; ++i)
			if (buckets[i].head != nullptr)
				return Iterator(buckets[i].head, this);
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, ValueBase, Hasher, Allocator>::ConstIterator VirtualMap<Key, ValueBase, Hasher, Allocator>::FirstIterator() const
	{
		if (bucketCount == 0)
			return ConstIterator();

		for (uintMem i = 0; i < bucketCount; ++i)
			if (buckets[i].head != nullptr)
				return ConstIterator(buckets[i].head, this);
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, ValueBase, Hasher, Allocator>::Iterator VirtualMap<Key, ValueBase, Hasher, Allocator>::LastIterator()
	{
		if (bucketCount == 0)
			return Iterator();

		for (uintMem i = bucketCount - 1; i >= 0; --i)
			if (buckets[i].head != nullptr)
				return Iterator(buckets[i].tail, this);
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, ValueBase, Hasher, Allocator>::ConstIterator VirtualMap<Key, ValueBase, Hasher, Allocator>::LastIterator() const
	{
		if (bucketCount == 0)
			return ConstIterator();

		for (uintMem i = bucketCount - 1; i >= 0; --i)
			if (buckets[i].head != nullptr)
				return ConstIterator(buckets[i].tail, this);
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, ValueBase, Hasher, Allocator>::Iterator VirtualMap<Key, ValueBase, Hasher, Allocator>::AheadIterator()
	{
		return Iterator();
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, ValueBase, Hasher, Allocator>::ConstIterator VirtualMap<Key, ValueBase, Hasher, Allocator>::AheadIterator() const
	{
		return ConstIterator();
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, ValueBase, Hasher, Allocator>::Iterator VirtualMap<Key, ValueBase, Hasher, Allocator>::BehindIterator()
	{
		return Iterator();
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, ValueBase, Hasher, Allocator>::ConstIterator VirtualMap<Key, ValueBase, Hasher, Allocator>::BehindIterator() const
	{
		return ConstIterator();
	}	
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, ValueBase, Hasher, Allocator>& VirtualMap<Key, ValueBase, Hasher, Allocator>::operator=(VirtualMap&& other) noexcept
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
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, ValueBase, Hasher, Allocator>::NodeHeader::NodeHeader(NodeHeader* prev, NodeHeader* next, uintMem hash, Key&& key)
		: prev(prev), next(next), hash(hash), key(std::move(key))
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		, iteratorCount(0)
#endif		
	{
	}	
	template<typename Key, typename ValueBaseBase, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, ValueBaseBase, Hasher, Allocator>::NodeHeader::~NodeHeader()
	{
	}	
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	template<typename Value> requires InsertableToVirtualMap<Value, ValueBase>
	template<typename ... Args> requires std::constructible_from<Value, Args...>
	inline VirtualMap<Key, ValueBase, Hasher, Allocator>::Node<Value>::Node(NodeHeader* prev, NodeHeader* next, uintMem hash, Key&& key, Args&& ... args)
		: NodeHeader(prev, next, hash, std::move(key)), value(std::forward<Args>(args)...)
	{
	}	
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	template<typename Value> requires InsertableToVirtualMap<Value, ValueBase>
	inline VirtualMap<Key, ValueBase, Hasher, Allocator>::Node<Value>::~Node()
	{
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline uintMem VirtualMap<Key, ValueBase, Hasher, Allocator>::Hash(const Key& key)
	{
		return Hasher::Compute(key);
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, ValueBase, Hasher, Allocator>::Bucket* VirtualMap<Key, ValueBase, Hasher, Allocator>::GetBucketFromHash(uintMem hash) const
	{
		if (bucketCount == 0)
			return nullptr;

		return GetBucketFromHashModUnsafe(hash % bucketCount);
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, ValueBase, Hasher, Allocator>::Bucket* VirtualMap<Key, ValueBase, Hasher, Allocator>::GetBucketFromHashModUnsafe(uintMem hashMod) const
	{
		return &buckets[hashMod];
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, ValueBase, Hasher, Allocator>::Iterator VirtualMap<Key, ValueBase, Hasher, Allocator>::FindWithHint(const Key& key, uintMem hash)
	{
		auto* bucket = GetBucketFromHash(hash);

		if (bucket == nullptr) //Not a error. If we were calling Find on an empty map it shouldn't return an error
			return Iterator();

		return FindWithHintUnsafe(key, bucket);
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, ValueBase, Hasher, Allocator>::ConstIterator VirtualMap<Key, ValueBase, Hasher, Allocator>::FindWithHint(const Key& key, uintMem hash) const
	{
		auto* bucket = GetBucketFromHash(hash);

		if (bucket == nullptr) //Not a error. If we were calling Find on an empty map it shouldn't return an error
			return ConstIterator();

		return FindWithHintUnsafe(key, bucket);
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, ValueBase, Hasher, Allocator>::Iterator VirtualMap<Key, ValueBase, Hasher, Allocator>::FindWithHintUnsafe(const Key& key, Bucket* bucket)
	{
		NodeHeader* nodeHeader = bucket->head;

		if (nodeHeader == nullptr)
			return Iterator();

		while (nodeHeader->key != key)
		{
			nodeHeader = nodeHeader->next;

			if (nodeHeader == nullptr)
				return Iterator();
		}

		return Iterator(nodeHeader, this);
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, ValueBase, Hasher, Allocator>::ConstIterator VirtualMap<Key, ValueBase, Hasher, Allocator>::FindWithHintUnsafe(const Key& key, Bucket* bucket) const
	{
		NodeHeader* nodeHeader = bucket->head;

		if (nodeHeader == nullptr)
			return ConstIterator();

		while (nodeHeader->key != key)
		{
			nodeHeader = nodeHeader->next;

			if (nodeHeader == nullptr)
				return ConstIterator();
		}

		return ConstIterator(nodeHeader, this);
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	template<bool Replace, typename _Key, typename Value, typename ... Args> requires InsertableToVirtualMap<Value, ValueBase> && std::constructible_from<Key, _Key> && std::constructible_from<Value, Args...>
	inline VirtualMap<Key, ValueBase, Hasher, Allocator>::InsertResult VirtualMap<Key, ValueBase, Hasher, Allocator>::InsertWithHint(_Key&& key, uintMem hash, Args&& ... args)
	{
		if (count == 0)
			AllocateEmptyUnsafe();

		Bucket* bucket = GetBucketFromHash(hash);

		if (bucket == nullptr)
		{
			Debug::Logger::LogFatal("Blaze Engine", "Trying to insert a map pair but there are no buckets");
			return { Iterator(), false };
		}

		return InsertWithHintUnsafe<Replace, _Key, Value>(std::forward<_Key>(key), bucket, hash, std::forward<Args>(args)...);
	}	
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	template<bool Replace, typename _Key, typename Value, typename ... Args> requires InsertableToVirtualMap<Value, ValueBase> && std::constructible_from<Key, _Key> && std::constructible_from<Value, Args...>
	inline VirtualMap<Key, ValueBase, Hasher, Allocator>::InsertResult VirtualMap<Key, ValueBase, Hasher, Allocator>::InsertWithHintUnsafe(_Key&& key, Bucket* bucket, uintMem hash, Args&& ... args)
	{
		//TODO do the same for normal Map strucure:
		NodeHeader* nodeHeader = nullptr;
		NodeHeader* prev = nullptr;
		NodeHeader* next = nullptr;

		if (bucket->head == nullptr)
		{
			bucket->head = (NodeHeader*)allocator.Allocate(sizeof(Node<Value>));
			bucket->tail = bucket->head;

			++count;

			nodeHeader = bucket->head;			
		}
		else
		{
			nodeHeader = bucket->head;

			bool found = false;
			while (true)
			{
				if constexpr (!Replace)
				{
					if (nodeHeader->key == key)
						return { Iterator(nodeHeader, this), false };
				}
				else
				{
					if (nodeHeader->key == key)
					{
						prev = nodeHeader->prev;
						next = nodeHeader->next;
						
						std::destroy_at(nodeHeader);
						allocator.Free(nodeHeader);

						nodeHeader = (NodeHeader*)allocator.Allocate(sizeof(Node<Value>));;

						if (prev != nullptr)
							prev->next = nodeHeader;
						if (next != nullptr)
							next->prev = nodeHeader;

						found = true;
						
						break;
					}
				}

				if (nodeHeader->next == nullptr)
					break;

				nodeHeader = nodeHeader->next;
			}

			if (!found)
			{
				nodeHeader->next = (NodeHeader*)allocator.Allocate(sizeof(Node<Value>));
				bucket->tail = nodeHeader->next;

				++count;

				prev = nodeHeader;
				nodeHeader = nodeHeader->next;
			}
		}
		
		std::construct_at((Node<Value>*)nodeHeader, prev, next, hash, std::forward<_Key>(key), std::forward<Args>(args)...);

		CheckForRehash();

		return { Iterator(nodeHeader, this), true };
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline void VirtualMap<Key, ValueBase, Hasher, Allocator>::EraseNodeUnsafe(Bucket* bucket, NodeHeader* nodeHeader)
	{
		RemoveNodeFromSpotUnsafe(nodeHeader, bucket);

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK				
		if (nodeHeader->iteratorCount)
			Debug::Logger::LogWarning("Blaze Engine", "Erasing a map node while a iterator is referencing it");
#endif

		std::destroy_at(nodeHeader);
		allocator.Free(nodeHeader);
		--count;
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline void VirtualMap<Key, ValueBase, Hasher, Allocator>::CheckForRehash()
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

			return Rehash(newBucketCount);
		}
		
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline void VirtualMap<Key, ValueBase, Hasher, Allocator>::Rehash(uintMem newBucketCount)
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
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline void VirtualMap<Key, ValueBase, Hasher, Allocator>::AllocateEmptyUnsafe()
	{
		bucketCount = 8;
		buckets = (Bucket*)allocator.Allocate(sizeof(Bucket) * bucketCount);
		memset(buckets, 0, sizeof(Bucket) * bucketCount);
		count = 0;
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline void VirtualMap<Key, ValueBase, Hasher, Allocator>::RehashBucketUnsafe(Bucket* bucket, Bucket* bucketArray, uintMem bucketArraySize)
	{
		NodeHeader* node = bucket->head;

		while (node != nullptr)
		{
			NodeHeader* next = node->next;

			uintMem newHashMod = node->hash % bucketArraySize;
			Bucket* newBucket = &bucketArray[newHashMod];

			MoveNodeToBeginingUnsafe(node, newBucket);

			node = next;
		}		
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline void VirtualMap<Key, ValueBase, Hasher, Allocator>::RemoveNodeFromSpotUnsafe(NodeHeader* nodeHeader, Bucket* bucket)
	{
		if (nodeHeader->prev)
		{
			NodeHeader* prev = nodeHeader->prev;

			if (nodeHeader->next)
			{
				NodeHeader* next = nodeHeader->next;

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
			NodeHeader* next = nodeHeader->next;

			if (nodeHeader->next)
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
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	inline void VirtualMap<Key, ValueBase, Hasher, Allocator>::MoveNodeToBeginingUnsafe(NodeHeader* nodeHeader, Bucket* bucket)
	{
		NodeHeader* head = bucket->head;
		nodeHeader->next = head;
		nodeHeader->prev = nullptr;

		if (head != nullptr)
			head->prev = nodeHeader;
		else
			bucket->tail = nodeHeader;

		bucket->head = nodeHeader;
	}

	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	VirtualMap<Key, ValueBase, Hasher, Allocator>::Iterator begin(VirtualMap<Key, ValueBase, Hasher, Allocator>& map)
	{
		return map.FirstIterator();
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	VirtualMap<Key, ValueBase, Hasher, Allocator>::ConstIterator begin(const VirtualMap<Key, ValueBase, Hasher, Allocator>& map)
	{
		return map.FirstIterator();
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	VirtualMap<Key, ValueBase, Hasher, Allocator>::Iterator end(VirtualMap<Key, ValueBase, Hasher, Allocator>& map)
	{
		return map.BehindIterator();
	}
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	VirtualMap<Key, ValueBase, Hasher, Allocator>::ConstIterator end(const VirtualMap<Key, ValueBase, Hasher, Allocator>& map)
	{
		return map.BehindIterator();
	}
}