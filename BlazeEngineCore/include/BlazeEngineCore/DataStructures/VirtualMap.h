#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"

namespace Blaze
{
	template<typename Key, typename ValueBase, typename Hasher, AllocatorType Allocator>
	class VirtualMap;

	template<typename T1, typename T2>
	concept IsConvertibleToVirtualMapIterator =
		std::same_as<std::remove_const_t<typename T2::template VirtualMapType>, std::remove_const_t<typename T1::template VirtualMapType>> &&
		(!(std::is_const_v<typename T1::template VirtualMapType> && !std::is_const_v<typename T2::template VirtualMapType>));

	template<typename T1, typename T2>
	concept IsComparableToVirtualMapIterator =
		std::same_as<std::remove_const_t<typename T2::template VirtualMapType>, std::remove_const_t<typename T1::template VirtualMapType>>;

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
	template<typename VirtualMap>
	class BLAZE_CORE_API VirtualMapIterator
	{
	public:		
		using VirtualMapType = VirtualMap;				

		VirtualMapIterator();
		template<IsConvertibleToVirtualMapIterator<VirtualMapIterator<VirtualMap>> T>
		VirtualMapIterator(const T&);		

		bool IsNull() const;

		VirtualMapIterator& operator++();
		VirtualMapIterator operator++(int);
		VirtualMapIterator& operator--();
		VirtualMapIterator operator--(int);

		//Returns the value that the iterator is pointing to. If the asked type does not match the stored value nullptr is returned. If 
		//the iterator isn't pointing to any data nullptr is returned
		template<typename Value> requires InsertableToVirtualMap<Value, typename VirtualMap::template ValueBaseType>
		Value* GetValue() requires !std::is_const_v<VirtualMap>;
		//Returns the value that the iterator is pointing to. If the asked type does not match the stored value nullptr is returned. If 
		//the iterator isn't pointing to any data nullptr is returned
		template<typename Value> requires InsertableToVirtualMap<Value, typename VirtualMap::template ValueBaseType>
		const Value* GetValue() const;
		//Returns the key that the iterator is pointing to. If the iterator isn't pointing to any data nullptr is returned				
		const typename VirtualMap::template KeyType* GetKey() const;

		template<IsComparableToVirtualMapIterator<VirtualMapIterator<VirtualMap>> T>
		bool operator==(const T&) const;
		template<IsComparableToVirtualMapIterator<VirtualMapIterator<VirtualMap>> T>
		bool operator!=(const T&) const;

		template<IsConvertibleToVirtualMapIterator<VirtualMapIterator<VirtualMap>> T>
		VirtualMapIterator& operator=(const T&);

		template<typename, typename, typename, AllocatorType>
		friend class ::Blaze::VirtualMap;

		template<typename>
		friend class ::Blaze::VirtualMapIterator;		
	private:
		using VirtualMapNodeHeader = std::conditional_t<std::is_const_v<VirtualMap>, const typename VirtualMap::template NodeHeader, typename VirtualMap::template NodeHeader>;
		using VirtualMapBucket = std::conditional_t<std::is_const_v<VirtualMap>, const typename VirtualMap::template Bucket, typename VirtualMap::template Bucket>;

		VirtualMap* map;
		VirtualMapNodeHeader* nodeHeader;

		VirtualMapIterator(VirtualMapNodeHeader* nodeHeader, VirtualMap* map);
	};		

	template<typename Value, typename ValueBase>
	concept InsertableToVirtualMap = std::derived_from<Value, ValueBase> || std::same_as<ValueBase, void>;

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
	template<typename Key, typename ValueBase = void, typename Hasher = ::Blaze::Hash<Key>, AllocatorType Allocator = Blaze::DefaultAllocator>
	class BLAZE_CORE_API VirtualMap
	{
	public:
		using KeyType = Key;
		using ValueBaseType = ValueBase;
		using HasherType = Hasher;
		using AllocatorType = Allocator;
		
		using Iterator = VirtualMapIterator<VirtualMap>;
		using ConstIterator = VirtualMapIterator<const VirtualMap>;
		
		struct InsertResult
		{
			Iterator iterator;
			bool inserted;
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
		template<typename Value, bool Replace = false, typename _Key, typename ... Args> requires InsertableToVirtualMap<Value, ValueBase> && std::constructible_from<Key, _Key>&& std::constructible_from<Value, Args...>
		InsertResult Insert(_Key&& key, Args&& ... args);
		bool Erase(const Key& key);
		bool Erase(const Iterator& iterator);

		uintMem Count() const;

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
		struct NodeHeader
		{
			uintMem hash;
			NodeHeader* next;
			NodeHeader* prev;
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
			uintMem iteratorCount;
#endif			
			Key key;

			NodeHeader(NodeHeader* prev, NodeHeader* next, uintMem hash, Key&& key);
			virtual ~NodeHeader();
		};
		template<typename Value> requires InsertableToVirtualMap<Value, ValueBase>
		struct Node : public NodeHeader
		{
			Value value;

			template<typename ... Args> requires std::constructible_from<Value, Args...>
			Node(NodeHeader* prev, NodeHeader* next, uintMem hash, Key&& key, Args&& ... args);
			~Node() override;
		};
		struct Bucket
		{
			NodeHeader* head;
			NodeHeader* tail;
		};

		uintMem bucketCount;
		Bucket* buckets;
		uintMem count;
		BLAZE_ALLOCATOR_ATTRIBUTE Allocator allocator;		

		static uintMem Hash(const Key& key);

		Bucket* GetBucketFromHash(uintMem hash) const;

		//Wont check if hashMod is smaller than bucketCount
		Bucket* GetBucketFromHashModUnsafe(uintMem hashMod) const;

		Iterator FindWithHint(const Key& key, uintMem hash);
		ConstIterator FindWithHint(const Key& key, uintMem hash) const;

		//Wont check if bucket is nullptr
		Iterator FindWithHintUnsafe(const Key& key, Bucket* bucket);
		ConstIterator FindWithHintUnsafe(const Key& key, Bucket* bucket) const;

		template<bool Replace, typename _Key, typename Value, typename ... Args> requires InsertableToVirtualMap<Value, ValueBase> && std::constructible_from<Key, _Key> && std::constructible_from<Value, Args...>
		InsertResult InsertWithHint(_Key&& key, uintMem hash, Args&& ... args);

		//Wont check if bucket is nullptr
		template<bool Replace, typename _Key, typename Value, typename ... Args> requires InsertableToVirtualMap<Value, ValueBase> && std::constructible_from<Key, _Key> && std::constructible_from<Value, Args...>
		InsertResult InsertWithHintUnsafe(_Key&& key, Bucket* bucket, uintMem hash, Args&& ... args);

		//Wont check if bucket is nullptr
		void EraseNodeUnsafe(Bucket* bucket, NodeHeader* node);

		void CheckForRehash();
		void Rehash(uintMem newBucketCount);

		//Doesn't deallocate previous memory
		void AllocateEmptyUnsafe();

		//Wont check if bucket or bucketArray is nullptr or bucketArraySize is 0
		static void RehashBucketUnsafe(Bucket* bucket, Bucket* bucketArray, uintMem bucketArraySize);

		//Wont check if bucket or node is nullptr
		static void RemoveNodeFromSpotUnsafe(NodeHeader* node, Bucket* bucket);

		//Wont check if oldBucket, newBucket or node is nullptr
		//static void MoveNodeUnsafe(Node* node, Bucket* oldBucket, Bucket* newBucket);

		//Wont check if node or bucket is nullptr or change old node neighbour pointers acordingly
		static void MoveNodeToBeginingUnsafe(NodeHeader* node, Bucket* bucket);

		template<typename>
		friend class VirtualMapIterator;
	};		
}