#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"
#include "BlazeEngineCore/Memory/Allocator.h"
#include "BlazeEngineCore/DataStructures/Hash.h"
#include <type_traits>

namespace Blaze
{
	template<HashableType Key, typename Base, typename Hasher, AllocatorType Allocator>
	class VirtualMap;

	template<HashableType Key, typename Base>
	struct BLAZE_CORE_API VirtualMapNodeBase
	{
		using KeyType = Key;
		using BaseType = Base;
		
		VirtualMapNodeBase* next;
		VirtualMapNodeBase* prev;
		Key key;

		template<typename _Key>
		VirtualMapNodeBase(VirtualMapNodeBase* prev, VirtualMapNodeBase* next, _Key&& key);
		virtual ~VirtualMapNodeBase();
	};

	template<HashableType Key, typename Base, typename Derived> requires (std::derived_from<Derived, Base> || std::same_as<Base, void>) && (!std::is_final_v<Derived>)
	struct BLAZE_CORE_API VirtualMapNode : public VirtualMapNodeBase<Key, Base>, public Derived
	{
		using DerivedType = Derived;

		template<typename _Key, typename ... Args> requires std::constructible_from<Derived, Args...>
		VirtualMapNode(VirtualMapNodeBase<Key, Base>* prev, VirtualMapNodeBase<Key, Base>* next, _Key&& key, Args&& ... args);
		~VirtualMapNode() override;
	};

	template<HashableType Key, typename Base>
	struct VirtualMapData
	{
		struct Bucket
		{
			VirtualMapNodeBase<Key, Base>* head;
			VirtualMapNodeBase<Key, Base>* tail;
		};

		Bucket* begin;
		Bucket* end;
	};

	/*
	* -----------VirtualMapIterator-----------
		Used with the Blaze::VirtualMap class.
		It points to a (kay, value) pair with a value of any type derived from VirtualMap::ValueBase.

		Iterator usage:
		Increasing, decreasing the null iterator, or dereferencing the iterator outside of valid range or invalidating the maps previous 
		buffers while some iterators are referencing it cannot be detected if no debugging measures are taken. It is up to the client to 
		ensure its valid use.

		Null iterators can be created by the default constructor, by copying/assigning an null iterator, or increasing\decreasing the iterator
		outside the map. They cannot be dereferenced, incremented or decremented, it will result in an error or a possible silent fail, depending
		if the BLAZE_NULL_ITERATOR_CHECK macro is defined.

		Erasing an element that no map iterator is referencing wont invalidate any map iterators.

		BLAZE_NULL_ITERATOR_CHECK:
		If BLAZE_NULL_ITERATOR_CHECK is defined, incrementing, decrementing or dereferencing a null map iterator will result in a fatal error.
		Otherwise it will try to continue execution and have a possible silent fail. Having this undefined will slightly improve performance.
		It IS defined in debug mode by default (it isn't defined in release mode).

		BLAZE_CONTAINER_INVALIDATION_CHECK:
		By changing a map element that an map iterator is pointing to (erasing, clearing, etc...) the iterator becomes invalid. If
		BLAZE_CONTAINER_INVALIDATION_CHECK is defined and any operation noted above is executed and there is a map iterator referencing the
		element, a fatal error will be logged. Otherwise it will try to continue execution and have a possible silent fail. When this macro
		is defined, the size of the nodes in the map change because additional information is needed to be stored for this macro to work.
		Having this undefined will improve performance. It is NOT defined in debug mode by default (not defined in release mode either).

		No other macros change the map iterator behaviour
	*/
	template<typename NodeBase>
	class BLAZE_CORE_API VirtualMapIterator
	{
	public:
		using NodeBaseType = NodeBase;
		using VirtualMapConstIterator = VirtualMapIterator<const NodeBase>;
		using KeyType = typename NodeBase::template KeyType;
		using BaseType = typename NodeBase::template BaseType;
		template<typename T>
		using MatchConst = std::conditional_t<std::is_const_v<NodeBase>, const T, T>;

		VirtualMapIterator();		
		VirtualMapIterator(const VirtualMapIterator& other);
		VirtualMapIterator(const VirtualMapConstIterator& other) requires (!std::is_const_v<VirtualMapIterator>);
		VirtualMapIterator(MatchConst<VirtualMapData<KeyType, BaseType>>& virtualMapData, typename VirtualMapData<KeyType, BaseType>::template Bucket* bucket, NodeBase* nodeBase);

		bool IsNull() const;

		template<typename Derived>
		MatchConst<Derived>* GetValue() const requires std::derived_from<Derived, BaseType> || std::same_as<BaseType, void>;
		const KeyType& GetKey() const;		

		VirtualMapIterator& operator++();
		VirtualMapIterator operator++(int);
		VirtualMapIterator& operator--();
		VirtualMapIterator operator--(int);
				
		bool operator==(const VirtualMapIterator& other) const;
		bool operator==(const VirtualMapConstIterator& other) const requires (!std::is_const_v<VirtualMapIterator>);
		bool operator!=(const VirtualMapIterator& other) const;
		bool operator!=(const VirtualMapConstIterator& other) const requires (!std::is_const_v<VirtualMapIterator>);
		
		VirtualMapIterator& operator=(const VirtualMapIterator& other);
		VirtualMapIterator& operator=(const VirtualMapConstIterator& other) requires (!std::is_const_v<VirtualMapIterator>);

		template<HashableType, typename, typename, AllocatorType>
		friend class VirtualMap;
	private:	
		MatchConst<VirtualMapData<KeyType, BaseType>>* mapData;
		typename VirtualMapData<KeyType, BaseType>::template Bucket* bucket;
		NodeBase* nodeBase;
	};			

	/*
		Virtual map class.
		Maps any object that is derived from ValueBase to Key, while checking for type correctness on access. If the ValueBase is
		void, then any type can be inserted.

		BLAZE_INVALID_ITERATOR_CHECK:
		If BLAZE_INVALID_ITERATOR_CHECK is defined, passing an invalid iterator to a map will result in a fatal error. Having this
		undefined will slightly improve performance. It IS defined in debug mode by default (not defined in release mode).

		BLAZE_CONTAINER_INVALIDATION_CHECK:
		By changing a map element that an map iterator is pointing to (erasing, clearing, etc...) the iterator becomes invalid. If
		BLAZE_CONTAINER_INVALIDATION_CHECK is defined and any operation noted above is executed and there is a map iterator referencing the
		element, a fatal error will be logged. Otherwise it will try to continue execution and have a possible silent fail. When this macro
		is defined, the size of the nodes in the map change because additional information is needed to be stored for this macro to work.
		Having this undefined will improve performance. It is NOT defined in debug mode by default (not defined in release mode either).

		No other macros change the map behaviour
	*/
	template<HashableType Key, typename Base = void, typename Hasher = ::Blaze::Hash<Key>, AllocatorType Allocator = Blaze::DefaultAllocator>
	class BLAZE_CORE_API VirtualMap : private VirtualMapData<Key, Base>
	{
	public:
		using KeyType = Key;
		using BaseType = Base;
		using HasherType = Hasher;
		using AllocatorType = Allocator;
		using NodeBaseType = VirtualMapNodeBase<Key, Base>;
		
		using Iterator = VirtualMapIterator<NodeBaseType>;
		using ConstIterator = VirtualMapIterator<const NodeBaseType>;
		
		template<typename Derived>
		struct InsertResult
		{
			Iterator iterator;
			bool inserted;
			Derived* value;
		};

		VirtualMap();		
		VirtualMap(const VirtualMap&) = delete;
		VirtualMap(VirtualMap&&) noexcept;		

		~VirtualMap();

		void Clear();
		bool Empty() const;

		Iterator Find(const Key& key);
		ConstIterator Find(const Key& key) const;

		bool Contains(const Key& key) const;

		/*
			Insert a element into the virtual map. If the Replace template argument is false and there is already an element 
			with the same key, nothing will be changed. If the Replace template argument is true and there is already an element 
			with the same key, the value will be destructed and a new one will be constructed.

			The template argument Value is mandatory, other arguments can be derived. The element being inserted must be of
			type that is derived from ValueBase, or if ValueBase is void then any type can be inserted.
			
			The default value of the template argument Replace is false.
		\returns
			Returns a iterator that points to the (key, value) pair. A null iterator is returned if there already was a value
			with the same key.
		*/
		template<typename Derived, bool Replace = false, typename _Key, typename ... Args> requires (std::derived_from<Derived, Base> || std::same_as<Base, void>) && std::constructible_from<Key, _Key>&& std::constructible_from<Derived, Args...>
		InsertResult<Derived> Insert(_Key&& key, Args&& ... args);
		bool Erase(const Key& key);
		bool Erase(const Iterator& iterator);

		/*
			Returns an iterator pointing to the first element in the map. If the map is empty returns a null map iterator.
		*/
		Iterator FirstIterator();
		/*
			Returns an iterator pointing to the first element in the map. If the map is empty returns a null map iterator.
		*/
		ConstIterator FirstIterator() const;
		/*
			Returns an iterator pointing to the last element in the map. If the map is empty returns a null map iterator.
		*/
		Iterator LastIterator();
		/*
			Returns an iterator pointing to the last element in the map. If the map is empty returns a null map iterator.
		*/
		ConstIterator LastIterator() const;
		/*
			Returns a null iterator always.
		*/
		Iterator AheadIterator();
		/*
			Returns a null iterator always.
		*/
		ConstIterator AheadIterator() const;
		/*
			Returns a null iterator always.
		*/
		Iterator BehindIterator();
		/*
			Returns a null iterator always.
		*/
		ConstIterator BehindIterator() const;
		
		VirtualMap& operator=(const VirtualMap&) = delete;
		VirtualMap& operator=(VirtualMap&&) noexcept;

	private:		
		using Bucket = typename VirtualMapData<Key, Base>::template Bucket;

		BLAZE_ALLOCATOR_ATTRIBUTE Allocator allocator;		

		static uintMem Hash(const Key& key);

		Bucket* GetBucketFromHash(uintMem hash) const;

		//Wont check if hashMod is smaller than bucketCount
		Bucket* GetBucketFromHashModUnsafe(uintMem hash) const;

		Iterator FindWithHint(const Key& key, uintMem hash);
		ConstIterator FindWithHint(const Key& key, uintMem hash) const;

		//Wont check if bucket is nullptr
		Iterator FindWithHintUnsafe(const Key& key, Bucket* bucket);
		ConstIterator FindWithHintUnsafe(const Key& key, Bucket* bucket) const;

		template<bool Replace, typename _Key, typename Derived, typename ... Args> requires (std::derived_from<Derived, Base> || std::same_as<Base, void>) && std::constructible_from<Key, _Key> && std::constructible_from<Derived, Args...>
		InsertResult<Derived> InsertWithHint(_Key&& key, uintMem hash, Args&& ... args);

		//Wont check if bucket is nullptr
		template<bool Replace, typename _Key, typename Derived, typename ... Args> requires (std::derived_from<Derived, Base> || std::same_as<Base, void>) && std::constructible_from<Key, _Key> && std::constructible_from<Derived, Args...>
		InsertResult<Derived>InsertWithHintUnsafe(_Key&& key, Bucket* bucket, uintMem hash, Args&& ... args);

		//Wont check if bucket is nullptr
		void EraseNodeUnsafe(Bucket* bucket, VirtualMapNodeBase<Key, BaseType>* node);

		void CheckForRehash();
		void Rehash(uintMem newBucketCount);

		//Doesn't deallocate previous memory
		void AllocateEmptyUnsafe();

		//Wont check if bucket or bucketArray is nullptr or bucketArraySize is 0
		static void RehashBucketUnsafe(Bucket* bucket, Bucket* bucketArray, uintMem bucketArraySize);

		//Wont check if bucket or node is nullptr
		static void RemoveNodeFromSpotUnsafe(VirtualMapNodeBase<Key, BaseType>* node, Bucket* bucket);

		//Wont check if oldBucket, newBucket or node is nullptr
		//static void MoveNodeUnsafe(Node* node, Bucket* oldBucket, Bucket* newBucket);

		//Wont check if node or bucket is nullptr or change old node neighbour pointers acordingly
		static void MoveNodeToBeginingUnsafe(VirtualMapNodeBase<Key, BaseType>* node, Bucket* bucket);
	};		

	template<HashableType Key, typename Base, typename Hasher, AllocatorType Allocator>
	VirtualMap<Key, Base, Hasher, Allocator>::Iterator begin(VirtualMap<Key, Base, Hasher, Allocator>& map)
	{
		return map.FirstIterator();
	}
	template<HashableType Key, typename Base, typename Hasher, AllocatorType Allocator>
	VirtualMap<Key, Base, Hasher, Allocator>::ConstIterator begin(const VirtualMap<Key, Base, Hasher, Allocator>& map)
	{
		return map.FirstIterator();
	}
	template<HashableType Key, typename Base, typename Hasher, AllocatorType Allocator>
	VirtualMap<Key, Base, Hasher, Allocator>::Iterator end(VirtualMap<Key, Base, Hasher, Allocator>& map)
	{
		return map.BehindIterator();
	}
	template<HashableType Key, typename Base, typename Hasher, AllocatorType Allocator>
	VirtualMap<Key, Base, Hasher, Allocator>::ConstIterator end(const VirtualMap<Key, Base, Hasher, Allocator>& map)
	{
		return map.BehindIterator();
	}			
}