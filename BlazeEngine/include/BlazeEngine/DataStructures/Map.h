#pragma once

namespace Blaze
{
	class Result;

	template<typename Key, typename Value, typename Hasher, AllocatorType Allocator>
	class Map;

	template<typename Key, typename Value>
	struct BLAZE_API MapPair
	{		
		using KeyType = Key;
		using ValueType = Value;

		Key key;
		Value value;

		MapPair() : key(), value() { }				
		MapPair(Key&& key, Value&& value) : key(std::move(key)), value(std::move(value)) { }
	};

	template<typename T1, typename T2>
	concept IsConvertibleToMapIterator =
		std::same_as<std::remove_const_t<typename T2::template MapType>, std::remove_const_t<typename T1::template MapType>> &&
		(!(std::is_const_v<typename T1::template MapType> && !std::is_const_v<typename T2::template MapType>));

	template<typename T1, typename T2>
	concept IsComparableToMapIterator =
		std::same_as<std::remove_const_t<typename T2::template MapType>, std::remove_const_t<typename T1::template MapType>>;

	/*
		Used with the Blaze::Map class.

		Iterator usage:
		Increasing, decreasing or dereferencing the iterator outside of valid range or invalidating the maps previous buffers while some iterators
		are referencing it cannot be detected if no debugging measures are taken. It is up to the client to ensure its valid use. 				

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

		Known issues:
		the key is changable when returned in the MapPair from the iterator.
	*/
	template<typename Map>
	class BLAZE_API MapIterator
	{
	public:
		using MapType = Map;
		using MapPairType = std::conditional_t<std::is_const_v<Map>, const typename Map::template MapPairType, typename Map::template MapPairType>;		

		MapIterator();						
		template<IsConvertibleToMapIterator<MapIterator<Map>> T>
		MapIterator(const T&);		

		bool IsNull() const;		

		MapIterator& operator++();
		MapIterator operator++(int);		
		MapIterator& operator--();
		MapIterator operator--(int);

		MapPairType& operator*() const;
		MapPairType* operator->() const;

		template<IsComparableToMapIterator<MapIterator<Map>> T>
		bool operator==(const T&) const;
		template<IsComparableToMapIterator<MapIterator<Map>> T>
		bool operator!=(const T&) const;

		template<IsConvertibleToMapIterator<MapIterator<Map>> T>
		MapIterator& operator=(const T&);
		
		template<typename, typename, typename, AllocatorType>
		friend class ::Blaze::Map;

		template<typename>
		friend class ::Blaze::MapIterator;
	private:		
		using MapNode = std::conditional_t<std::is_const_v<Map>, const typename Map::template Node, typename Map::template Node>;
		using MapBucket = std::conditional_t<std::is_const_v<Map>, const typename Map::template Bucket, typename Map::template Bucket>;

		Map* map;
		MapNode* node;		

		MapIterator(MapNode* node, Map* map);
	};

	/*
		Basic map class

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
	template<typename Key, typename Value, typename Hasher = ::Blaze::Hash<Key>, AllocatorType Allocator = Blaze::DefaultAllocator>
	class BLAZE_API Map
	{
	public:		
		using KeyType = Key;
		using ValueType = Value;
		using HasherType = Hasher;
		using AllocatorType = Allocator;

		using MapPairType = MapPair<Key, Value>;
		using Iterator = MapIterator<Map>;
		using ConstIterator = MapIterator<const Map>;

		struct InsertResult
		{
			Iterator iterator;
			bool inserted;
		};

		Map();
		Map(const Map&);		
		Map(Map&&) noexcept;

		Map(const std::initializer_list<MapPairType>&);

		~Map();

		void Clear();
		bool Empty() const;

		Iterator Find(const Key& key);
		ConstIterator Find(const Key& key) const;

		bool Contains(const Key& key) const;
		
		/*
			Inserst a element into the map. If there is already an element with the same key it doesnt change anything.

		\returns
			Returns a iterator that points to a pair associated with the key, and true if there wasnt a element with the
			same key and false otherwise.
		*/		
		InsertResult Insert(const Key& key, const Value& value);
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

		Map& operator=(const Map&);
		Map& operator=(Map&&) noexcept;

	private:		
		struct Node
		{
			uintMem hash;
			Node* next;
			Node* prev;
			MapPairType pair;
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
			uintMem iteratorCount;
#endif

			Node(Node* prev, Node* next, uintMem hash, Key&& key, Value&& value);
		};
		struct Bucket
		{
			Node* head;
			Node* tail;
		};

		uintMem bucketCount;
		Bucket* buckets;
		uintMem count;
		BLAZE_ALLOCATOR_ATTRIBUTE Allocator allocator;

		//Wont free previous contents
		void CopyUnsafe(const Map& other);

		static uintMem Hash(const Key& key);

		Bucket* GetBucketFromHash(uintMem hash) const;

		//Wont check if hashMod is smaller than bucketCount
		Bucket* GetBucketFromHashModUnsafe(uintMem hashMod) const;

		Iterator FindWithHint(const Key& key, uintMem hash);
		ConstIterator FindWithHint(const Key& key, uintMem hash) const;

		//Wont check if bucket is nullptr
		Iterator FindWithHintUnsafe(const Key& key, Bucket* bucket);
		ConstIterator FindWithHintUnsafe(const Key& key, Bucket* bucket) const;		

		InsertResult InsertWithHint(const Key& key, const Value& value, uintMem hash);

		//Wont check if bucket is nullptr
		InsertResult InsertWithHintUnsafe(const Key& key, const Value& value, Bucket* bucket, uintMem hash);

		//Wont check if bucket is nullptr
		void EraseNodeUnsafe(Bucket* bucket, Node* node);

		Result CheckForRehash();
		Result Rehash(uintMem newBucketCount);

		//Doesn't deallocate previous memory
		void AllocateEmptyUnsafe();

		//Wont check if bucket or bucketArray is nullptr or bucketArraySize is 0
		static Result RehashBucketUnsafe(Bucket* bucket, Bucket* bucketArray, uintMem bucketArraySize);

		//Wont check if bucket or node is nullptr
		static void RemoveNodeFromSpotUnsafe(Node* node, Bucket* bucket);

		//Wont check if oldBucket, newBucket or node is nullptr
		//static void MoveNodeUnsafe(Node* node, Bucket* oldBucket, Bucket* newBucket);
		
		//Wont check if node or bucket is nullptr or change old node neighbour pointers acordingly
		static void MoveNodeToBeginingUnsafe(Node* node, Bucket* bucket);

		template<typename>
		friend class MapIterator;
	};
}