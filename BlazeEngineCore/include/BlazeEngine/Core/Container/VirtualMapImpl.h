#pragma once
#include "BlazeEngine/Core/Debug/Logger.h"
#include <type_traits>
#include <memory>

namespace Blaze
{
	template<HashableType Key, typename Base>
	template<typename _Key>
	inline VirtualMapNodeBase<Key, Base>::VirtualMapNodeBase(VirtualMapNodeBase* prev, VirtualMapNodeBase* next, _Key&& key)
		: next(next), prev(prev), key(std::forward<_Key>(key))
	{
	}
	template<HashableType Key, typename Base>
	inline VirtualMapNodeBase<Key, Base>::~VirtualMapNodeBase()
	{
	}

	template<HashableType Key, typename Base, typename Derived> requires (IsDerivedFrom<Derived, Base> || SameAs<Base, void>) && (!IsFinal<Derived>)
	template<typename _Key, typename ... Args> requires IsConstructibleFrom<Derived, Args...>
	inline VirtualMapNode<Key, Base, Derived>::VirtualMapNode(VirtualMapNodeBase<Key, Base>* prev, VirtualMapNodeBase<Key, Base>* next, _Key&& key, Args && ...args)
		: VirtualMapNodeBase<Key, Base>(prev, next, std::forward<_Key>(key)), Derived(std::forward<Args>(args)...)
	{
	}
	template<HashableType Key, typename Base, typename Derived> requires (IsDerivedFrom<Derived, Base> || SameAs<Base, void>) && (!IsFinal<Derived>)
	inline VirtualMapNode<Key, Base, Derived>::~VirtualMapNode()
	{
	}

	template<typename NodeBase>
	inline VirtualMapIterator<NodeBase>::VirtualMapIterator()
		: mapData(nullptr), bucket(nullptr), nodeBase(nullptr)
	{
	}
	template<typename NodeBase>
	inline VirtualMapIterator<NodeBase>::VirtualMapIterator(const VirtualMapIterator& other)
		: mapData(other.mapData), bucket(other.bucket), nodeBase(other.nodeBase)
	{
	}
	template<typename NodeBase>
	inline VirtualMapIterator<NodeBase>::VirtualMapIterator(const VirtualMapConstIterator& other) requires (!IsConstType<VirtualMapIterator>)
		: mapData(other.mapData), bucket(other.bucket), nodeBase(other.nodeBase)
	{
	}
	template<typename NodeBase>
	inline VirtualMapIterator<NodeBase>::VirtualMapIterator(MatchConst<VirtualMapData<KeyType, BaseType>>& virtualMapData, typename VirtualMapData<KeyType, BaseType>::template Bucket* bucket, NodeBase* nodeBase)
		: mapData(&virtualMapData), bucket(bucket), nodeBase(nodeBase)
	{
	}
	template<typename NodeBase>
	inline bool VirtualMapIterator<NodeBase>::IsNull() const
	{
		return nodeBase == nullptr;
	}
	template<typename NodeBase>
	template<typename Derived>
	inline auto VirtualMapIterator<NodeBase>::GetValue() const -> MatchConst<Derived>* requires IsDerivedFrom<Derived, BaseType> || SameAs<BaseType, void>
	{
		if (nodeBase == nullptr)
		{
			BLAZE_LOG_FATAL("Trying to access a null iterator");
			return nullptr;
		}

		return dynamic_cast<MatchConst<Derived>*>(nodeBase);
	}
	template<typename NodeBase>
	inline auto VirtualMapIterator<NodeBase>::GetKey() const -> const KeyType&
	{
		if (nodeBase == nullptr)
		{
			BLAZE_LOG_FATAL("Trying to access a null iterator");
			return nullptr;
		}

		return nodeBase->key;
	}
	template<typename NodeBase>
	inline VirtualMapIterator<NodeBase>& VirtualMapIterator<NodeBase>::operator++()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (bucket != mapData->begin && nodeBase == nullptr)
			BLAZE_LOG_FATAL("Incrementing a null iterator");
#endif

		if (bucket == mapData->begin && nodeBase != nullptr || bucket != mapData->begin)
			nodeBase = nodeBase->next;

		while (nodeBase == nullptr)
		{
			++bucket;

			if (bucket == mapData->end)
			{
				nodeBase = nullptr;
				return *this;
			}

			nodeBase = bucket->head;
		}

		return *this;
	}
	template<typename NodeBase>
	inline VirtualMapIterator<NodeBase> VirtualMapIterator<NodeBase>::operator++(int)
	{
		auto copy = *this;
		++copy;
		return copy;
	}
	template<typename NodeBase>
	inline VirtualMapIterator<NodeBase>& VirtualMapIterator<NodeBase>::operator--()
	{
#ifdef BLAZE_NULL_ITERATOR_CHECK
		if (bucket != mapData->end && nodeBase == nullptr)
			BLAZE_LOG_FATAL("Decrementing a null iterator");
#endif

		if (bucket == mapData->end && nodeBase != nullptr || bucket != mapData->end)
			nodeBase = nodeBase->next;

		while (nodeBase == nullptr)
		{
			if (bucket == mapData->begin)
			{
				nodeBase = nullptr;
				return *this;
			}

			--bucket;

			nodeBase = bucket->tail;
		}

		return *this;
	}
	template<typename NodeBase>
	inline VirtualMapIterator<NodeBase> VirtualMapIterator<NodeBase>::operator--(int)
	{
		auto copy = *this;
		--copy;
		return copy;
	}
	template<typename NodeBase>
	inline bool VirtualMapIterator<NodeBase>::operator==(const VirtualMapIterator& other) const
	{
		return nodeBase == other.nodeBase && bucket == other.bucket;
	}
	template<typename NodeBase>
	inline bool VirtualMapIterator<NodeBase>::operator==(const VirtualMapConstIterator& other) const requires (!IsConstType<VirtualMapIterator>)
	{
		return nodeBase == other.nodeBase && bucket == other.bucket;
	}
	template<typename NodeBase>
	inline bool VirtualMapIterator<NodeBase>::operator!=(const VirtualMapIterator& other) const
	{
		return nodeBase != other.nodeBase || bucket != other.bucket;
	}
	template<typename NodeBase>
	inline bool VirtualMapIterator<NodeBase>::operator!=(const VirtualMapConstIterator& other) const requires (!IsConstType<VirtualMapIterator>)
	{
		return nodeBase != other.nodeBase || bucket != other.bucket;
	}
	template<typename NodeBase>
	inline VirtualMapIterator<NodeBase>& VirtualMapIterator<NodeBase>::operator=(const VirtualMapIterator& other)
	{
		mapData = other.mapData;
		bucket = other.bucket;
		nodeBase = other.nodeBase;
		return *this;
	}
	template<typename NodeBase>
	inline VirtualMapIterator<NodeBase>& VirtualMapIterator<NodeBase>::operator=(const VirtualMapConstIterator& other) requires (!IsConstType<VirtualMapIterator>)
	{
		mapData = other.mapData;
		bucket = other.bucket;
		nodeBase = other.nodeBase;
		return *this;
	}


	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, Base, Hasher, Allocator>::VirtualMap()
		: VirtualMapData<Key, Base>(nullptr, nullptr)
	{
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, Base, Hasher, Allocator>::VirtualMap(VirtualMap&& other) noexcept
		: VirtualMapData<Key, Base>(other)
	{
		other.begin = 0;
		other.end = nullptr;
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, Base, Hasher, Allocator>::~VirtualMap()
	{
		Clear();
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline void VirtualMap<Key, Base, Hasher, Allocator>::Clear()
	{
		for (Bucket* it = this->begin; it != this->end; ++it)
		{
			NodeBaseType* nodeBase = it->head;
			while (nodeBase != nullptr)
			{
				NodeBaseType* next = nodeBase->next;

				std::destroy_at(nodeBase);
				allocator.Free(nodeBase);
				nodeBase = next;
			}
		}

		allocator.Free(this->begin);

		this->begin = nullptr;
		this->end = nullptr;
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline bool VirtualMap<Key, Base, Hasher, Allocator>::Empty() const
	{
		return this->begin == nullptr;
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, Base, Hasher, Allocator>::Iterator VirtualMap<Key, Base, Hasher, Allocator>::Find(const Key& key)
	{
		return FindWithHint(key, Hash(key));
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, Base, Hasher, Allocator>::ConstIterator VirtualMap<Key, Base, Hasher, Allocator>::Find(const Key& key) const
	{
		return ConstIterator(FindWithHint(key, Hash(key)));
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline bool  VirtualMap<Key, Base, Hasher, Allocator>::Contains(const Key& key) const
	{
		uintMem hash = Hash(key);
		auto* bucket = GetBucketFromHash(hash);

		if (bucket == nullptr) //Not a error. If we were calling Contains on an empty set it shouldn't return an error
			return false;

		NodeBaseType* node = bucket->head;

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
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	template<typename Derived, bool Replace, typename _Key, typename ... Args>  requires (IsDerivedFrom<Derived, Base> || SameAs<Base, void>) && IsConstructibleFrom<Key, _Key>&& IsConstructibleFrom<Derived, Args...>
	inline VirtualMap<Key, Base, Hasher, Allocator>::InsertResult<Derived> VirtualMap<Key, Base, Hasher, Allocator>::Insert(_Key&& key, Args&& ... args)
	{
		return InsertWithHint<Replace, _Key, Derived>(std::forward<_Key>(key), Hash(key), std::forward<Args>(args)...);
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline bool VirtualMap<Key, Base, Hasher, Allocator>::Erase(const Key& key)
	{
		Bucket* bucket = GetBucketFromHash(Hash(key));

		if (bucket == nullptr)
			return false;

		auto it = FindWithHintUnsafe(key, bucket);

		if (it.IsNull())
			return false;

		EraseNodeUnsafe(bucket, it.nodeBase);

		return true;
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline bool VirtualMap<Key, Base, Hasher, Allocator>::Erase(const Iterator& iterator)
	{
#ifdef BLAZE_INVALID_ITERATOR_CHECK
		if (iterator.IsNull())
			return BLAZE_LOG_ERROR("Trying to erase a map pair with a null iterator");
#endif

		EraseNodeUnsafe(iterator.bucket, iterator.nodeBase);

		return true;
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, Base, Hasher, Allocator>::Iterator VirtualMap<Key, Base, Hasher, Allocator>::FirstIterator()
	{
		for (auto it = this->begin; it != this->end; ++it)
			if (it->head != nullptr)
				return Iterator(*this, it, it->head);

		return AheadIterator();
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, Base, Hasher, Allocator>::ConstIterator VirtualMap<Key, Base, Hasher, Allocator>::FirstIterator() const
	{
		for (auto it = this->begin; it != this->end; ++it)
			if (it->head != nullptr)
				return ConstIterator(*this, it, it->head);

		return AheadIterator();
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, Base, Hasher, Allocator>::Iterator VirtualMap<Key, Base, Hasher, Allocator>::LastIterator()
	{
		if (this->begin == nullptr)
			return BehindIterator();

		for (auto it = this->end - 1; it != this->begin - 1; --it)
			if (it->tail != nullptr)
				return Iterator(*this, it, it->tail);

		return BehindIterator();
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, Base, Hasher, Allocator>::ConstIterator VirtualMap<Key, Base, Hasher, Allocator>::LastIterator() const
	{
		if (this->begin == nullptr)
			return BehindIterator();

		for (auto it = this->end - 1; it != this->begin - 1; --it)
			if (it->tail != nullptr)
				return ConstIterator(*this, it, it->tail);

		return BehindIterator();
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, Base, Hasher, Allocator>::Iterator VirtualMap<Key, Base, Hasher, Allocator>::AheadIterator()
	{
		return Iterator(*this, this->begin, nullptr);
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, Base, Hasher, Allocator>::ConstIterator VirtualMap<Key, Base, Hasher, Allocator>::AheadIterator() const
	{
		return ConstIterator(*this, this->begin, nullptr);
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, Base, Hasher, Allocator>::Iterator VirtualMap<Key, Base, Hasher, Allocator>::BehindIterator()
	{
		return Iterator(*this, this->end, nullptr);
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, Base, Hasher, Allocator>::ConstIterator VirtualMap<Key, Base, Hasher, Allocator>::BehindIterator() const
	{
		return ConstIterator(*this, this->end, nullptr);
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, Base, Hasher, Allocator>& VirtualMap<Key, Base, Hasher, Allocator>::operator=(VirtualMap&& other) noexcept
	{
		Clear();

		this->begin = other.begin;
		this->end = other.end;
		other.begin = nullptr;
		other.end = nullptr;
		return *this;
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline uint64 VirtualMap<Key, Base, Hasher, Allocator>::Hash(const Key& key)
	{
		return Hasher().Compute(key);
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline auto VirtualMap<Key, Base, Hasher, Allocator>::GetBucketFromHash(uintMem hash) const -> Bucket*
	{
		if (this->begin == nullptr)
			return nullptr;

		return GetBucketFromHashModUnsafe(hash % (this->end - this->begin));
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline auto VirtualMap<Key, Base, Hasher, Allocator>::GetBucketFromHashModUnsafe(uintMem hashMod) const -> Bucket*
	{
		return &this->begin[hashMod];
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, Base, Hasher, Allocator>::Iterator VirtualMap<Key, Base, Hasher, Allocator>::FindWithHint(const Key& key, uintMem hash)
	{
		auto* bucket = GetBucketFromHash(hash);

		if (bucket == nullptr) //Not a error. If we were calling Find on an empty map it shouldn't return an error
			return Iterator();

		return FindWithHintUnsafe(key, bucket);
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, Base, Hasher, Allocator>::ConstIterator VirtualMap<Key, Base, Hasher, Allocator>::FindWithHint(const Key& key, uintMem hash) const
	{
		auto* bucket = GetBucketFromHash(hash);

		if (bucket == nullptr) //Not a error. If we were calling Find on an empty map it shouldn't return an error
			return ConstIterator();

		return FindWithHintUnsafe(key, bucket);
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, Base, Hasher, Allocator>::Iterator VirtualMap<Key, Base, Hasher, Allocator>::FindWithHintUnsafe(const Key& key, Bucket* bucket)
	{
		NodeBaseType* nodeBase = bucket->head;

		if (nodeBase == nullptr)
			return Iterator();

		while (nodeBase->key != key)
		{
			nodeBase = nodeBase->next;

			if (nodeBase == nullptr)
				return Iterator();
		}

		return Iterator(*this, bucket, nodeBase);
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline VirtualMap<Key, Base, Hasher, Allocator>::ConstIterator VirtualMap<Key, Base, Hasher, Allocator>::FindWithHintUnsafe(const Key& key, Bucket* bucket) const
	{
		NodeBaseType* nodeBase = bucket->head;

		if (nodeBase == nullptr)
			return ConstIterator();

		while (nodeBase->key != key)
		{
			nodeBase = nodeBase->next;

			if (nodeBase == nullptr)
				return ConstIterator();
		}

		return ConstIterator(*this, bucket, nodeBase);
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	template<bool Replace, typename _Key, typename Derived, typename ... Args> requires (IsDerivedFrom<Derived, Base> || SameAs<Base, void>) && IsConstructibleFrom<Key, _Key>&& IsConstructibleFrom<Derived, Args...>
	inline auto VirtualMap<Key, Base, Hasher, Allocator>::InsertWithHint(_Key&& key, uintMem hash, Args&& ... args) -> InsertResult<Derived>
	{
		if (this->begin == nullptr)
			AllocateEmptyUnsafe();

		Bucket* bucket = GetBucketFromHash(hash);

		if (bucket == nullptr)
		{
			BLAZE_LOG_FATAL("Trying to insert a map pair but there are no buckets");
			return { Iterator(), false };
		}

		return InsertWithHintUnsafe<Replace, _Key, Derived>(std::forward<_Key>(key), bucket, hash, std::forward<Args>(args)...);
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	template<bool Replace, typename _Key, typename Derived, typename ... Args> requires (IsDerivedFrom<Derived, Base> || SameAs<Base, void>) && IsConstructibleFrom<Key, _Key>&& IsConstructibleFrom<Derived, Args...>
	inline auto VirtualMap<Key, Base, Hasher, Allocator>::InsertWithHintUnsafe(_Key&& key, Bucket* bucket, uintMem hash, Args&& ... args) -> InsertResult<Derived>
	{
		NodeBaseType* nodeBase = nullptr;
		NodeBaseType* prev = nullptr;
		NodeBaseType* next = nullptr;

		if (bucket->head == nullptr)
		{
			bucket->head = (NodeBaseType*)allocator.Allocate(sizeof(VirtualMapNode<Key, Base, Derived>));
			bucket->tail = bucket->head;

			nodeBase = bucket->head;
		}
		else
		{
			nodeBase = bucket->head;

			bool found = false;
			while (true)
			{
				if constexpr (!Replace)
				{
					if (nodeBase->key == key)
						return { Iterator(*this, bucket, nodeBase), false, (Derived*)(VirtualMapNode<Key, Base, Derived>*)nodeBase };
				}
				else
				{
					if (nodeBase->key == key)
					{
						prev = nodeBase->prev;
						next = nodeBase->next;

						std::destroy_at(nodeBase);
						allocator.Free(nodeBase);

						nodeBase = (NodeBaseType*)allocator.Allocate(sizeof(VirtualMapNode<Key, Base, Derived>));;

						if (prev != nullptr)
							prev->next = nodeBase;
						if (next != nullptr)
							next->prev = nodeBase;

						found = true;

						break;
					}
				}

				if (nodeBase->next == nullptr)
					break;

				nodeBase = nodeBase->next;
			}

			if (!found)
			{
				nodeBase->next = (NodeBaseType*)allocator.Allocate(sizeof(VirtualMapNode<Key, Base, Derived>));
				bucket->tail = nodeBase->next;

				prev = nodeBase;
				nodeBase = nodeBase->next;
			}
		}

		std::construct_at((VirtualMapNode<Key, Base, Derived>*)nodeBase, prev, next, std::forward<_Key>(key), std::forward<Args>(args)...);

		CheckForRehash();

		return { Iterator(*this, bucket, nodeBase), true, (Derived*)(VirtualMapNode<Key, Base, Derived>*)nodeBase };
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline void VirtualMap<Key, Base, Hasher, Allocator>::EraseNodeUnsafe(Bucket* bucket, NodeBaseType* nodeBase)
	{
		RemoveNodeFromSpotUnsafe(nodeBase, bucket);

		std::destroy_at(nodeBase);
		allocator.Free(nodeBase);

		CheckForRehash();
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline void VirtualMap<Key, Base, Hasher, Allocator>::CheckForRehash()
	{
		static constexpr float max_load_factor = 1.0f;

		uintMem bucketCount = this->end - this->begin;
		uintMem count = 0;

		for (auto it = FirstIterator(); it != BehindIterator(); ++it)
			++count;

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
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline void VirtualMap<Key, Base, Hasher, Allocator>::Rehash(uintMem newBucketCount)
	{
		Bucket* oldBegin = this->begin;
		Bucket* oldEnd = this->end;

		this->begin = (Bucket*)allocator.Allocate(sizeof(Bucket) * newBucketCount);
		this->end = this->begin + newBucketCount;

		memset(this->begin, 0, sizeof(Bucket) * newBucketCount);

		for (Bucket* it = oldBegin; it != oldEnd; ++it)
			RehashBucketUnsafe(it, this->begin, newBucketCount);

		allocator.Free(oldBegin);
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline void VirtualMap<Key, Base, Hasher, Allocator>::AllocateEmptyUnsafe()
	{
		uintMem bucketCount = 8;
		this->begin = (Bucket*)allocator.Allocate(sizeof(Bucket) * bucketCount);
		this->end = this->begin + bucketCount;
		memset(this->begin, 0, sizeof(Bucket) * bucketCount);
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline void VirtualMap<Key, Base, Hasher, Allocator>::RehashBucketUnsafe(Bucket* bucket, Bucket* bucketArray, uintMem bucketArraySize)
	{
		NodeBaseType* node = bucket->head;

		while (node != nullptr)
		{
			NodeBaseType* next = node->next;

			uintMem newHashMod = Hash(node->key) % bucketArraySize;
			Bucket* newBucket = &bucketArray[newHashMod];

			MoveNodeToBeginingUnsafe(node, newBucket);

			node = next;
		}
	}
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline void VirtualMap<Key, Base, Hasher, Allocator>::RemoveNodeFromSpotUnsafe(NodeBaseType* nodeBase, Bucket* bucket)
	{
		if (nodeBase->prev)
		{
			NodeBaseType* prev = nodeBase->prev;

			if (nodeBase->next)
			{
				NodeBaseType* next = nodeBase->next;

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
			NodeBaseType* next = nodeBase->next;

			if (nodeBase->next)
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
	template<HashableType Key,typename Base, typename Hasher, AllocatorType Allocator>
	inline void VirtualMap<Key, Base, Hasher, Allocator>::MoveNodeToBeginingUnsafe(NodeBaseType* nodeBase, Bucket* bucket)
	{
		NodeBaseType* head = bucket->head;
		nodeBase->next = head;
		nodeBase->prev = nullptr;

		if (head != nullptr)
			head->prev = nodeBase;
		else
			bucket->tail = nodeBase;

		bucket->head = nodeBase;
	}
}