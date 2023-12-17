#pragma once

namespace Blaze
{
	template<typename Value, typename Hasher>
	concept ValidSetTemplateArguments =
		CanHashType<Hasher, Value>;

	template<typename Value, typename Hasher, AllocatorType> requires ValidSetTemplateArguments<Value, Hasher>
	class Set;	

	template<typename T1, typename T2>
	concept IsConvertibleToSetIterator =
		std::same_as<std::remove_const_t<typename T2::template SetType>, std::remove_const_t<typename T1::template SetType>> &&
		(!(std::is_const_v<typename T1::template SetType> && !std::is_const_v<typename T2::template SetType>));

	template<typename T1, typename T2>
	concept IsComparableToSetIterator =
		std::same_as<std::remove_const_t<typename T2::template SetType>, std::remove_const_t<typename T1::template SetType>>;

	/*
		Used with the Blaze::Set class.

		Iterator usage:
		Increasing, decreasing or dereferencing the iterator outside of valid range or invalidating the sets previous buffers while some iterators
		are referencing it cannot be detected if no debugging measures are taken. It is up to the client to ensure its valid use.

		Null iterators can be created by the default constructor, by copying/assigning an null iterator, or increasing\decreasing the iterator
		outside the set. They cannot be dereferenced, incremented or decremented, it will result in an error or a possible silent fail, depending
		if the BLAZE_NULL_ITERATOR_CHECK macro is defined.

		Erasing an element that no set iterator is referencing wont invalidate any set iterators.

		BLAZE_NULL_ITERATOR_CHECK:
		If BLAZE_NULL_ITERATOR_CHECK is defined, incrementing, decrementing or dereferencing a null set iterator will result in a fatal error.
		Otherwise it will try to continue execution and have a possible silent fail. Having this undefined will slightly improve performance.
		It IS defined in debug mode by default (it isn't defined in release mode).

		BLAZE_CONTAINER_INVALIDATION_CHECK:
		By changing a set element that an set iterator is pointing to (erasing, clearing, etc...) the iterator becomes invalid. If
		BLAZE_CONTAINER_INVALIDATION_CHECK is defined and any operation noted above is executed and there is a set iterator referencing the
		element, a fatal error will be logged. Otherwise it will try to continue execution and have a possible silent fail. When this macro
		is defined, the size of the nodes in the set change because additional information is needed to be stored for this macro to work.
		Having this undefined will improve performance. It is NOT defined in debug mode by default (not defined in release mode either).

		No other macros change the set iterator behaviour
	*/
	template<typename Set>
	class BLAZE_CORE_API SetIterator
	{
	public:
		using SetType = Set;
		using ValueType = std::conditional_t<std::is_const_v<Set>, const typename Set::template ValueType, typename Set::template ValueType>;
		using value_type = ValueType;

		SetIterator();
		template<IsConvertibleToSetIterator<SetIterator<Set>> T>
		SetIterator(const T&);

		bool IsNull() const;

		SetIterator& operator++();
		SetIterator operator++(int);
		SetIterator& operator--();
		SetIterator operator--(int);

		ValueType& operator*() const;
		ValueType* operator->() const;

		template<IsComparableToSetIterator<SetIterator<Set>> T>
		bool operator==(const T&) const;
		template<IsComparableToSetIterator<SetIterator<Set>> T>
		bool operator!=(const T&) const;

		template<IsConvertibleToSetIterator<SetIterator<Set>> T>
		SetIterator& operator=(const T&);

		template<typename Value, typename Hasher, AllocatorType> requires ValidSetTemplateArguments<Value, Hasher>
		friend class ::Blaze::Set;

		template<typename>
		friend class ::Blaze::SetIterator;
	private:
		using Node = std::conditional_t<std::is_const_v<Set>, const typename Set::template Node, typename Set::template Node>;
		using Bucket = std::conditional_t<std::is_const_v<Set>, const typename Set::template Bucket, typename Set::template Bucket>;

		Set* set;
		Node* node;

		SetIterator(Node* node, Set* set);
	};

	/*
		Basic set class

		BLAZE_INVALID_ITERATOR_CHECK:
		If BLAZE_INVALID_ITERATOR_CHECK is defined, passing an invalid iterator to a set will result in a fatal error. Having this
		undefined will slightly improve performance. It IS defined in debug mode by default (not defined in release mode).

		BLAZE_CONTAINER_INVALIDATION_CHECK:
		By changing a set element that an set iterator is pointing to (erasing, clearing, etc...) the iterator becomes invalid. If
		BLAZE_CONTAINER_INVALIDATION_CHECK is defined and any operation noted above is executed and there is a set iterator referencing the
		element, a fatal error will be logged. Otherwise it will try to continue execution and have a possible silent fail. When this macro
		is defined, the size of the nodes in the set change because additional information is needed to be stored for this macro to work.
		Having this undefined will improve performance. It is NOT defined in debug mode by default (not defined in release mode either).

		No other macros change the list behaviour
	*/
	template<typename Value, typename Hasher = ::Blaze::Hash<Value>, AllocatorType Allocator = Blaze::DefaultAllocator> requires ValidSetTemplateArguments<Value, Hasher>
	class BLAZE_CORE_API Set
	{
	public:		
		using ValueType = Value;
		using HasherType = Hasher;
		using AllocatorType = Allocator;
		
		using Iterator = SetIterator<Set>;
		using ConstIterator = SetIterator<const Set>;

		struct InsertResult
		{
			Iterator iterator;
			bool inserted;
		};		

		Set();
		Set(const Set&);
		Set(Set&&) noexcept;

		Set(const ArrayView<Value>&);
		Set(const std::initializer_list<Value>&);

		~Set();

		void Clear();
		bool Empty() const;

		Iterator Find(const Value& value);
		ConstIterator Find(const Value& value) const;

		bool Contains(const Value& value) const;

		/*
			Inserst a element into the set. If there is already an element with the same value it doesnt change anything.

		\returns
			Returns a iterator that points to the value, and true if there wasnt a element with the same value and false otherwise.
		*/
		InsertResult Insert(const Value& value);
		bool Erase(const Value& value);		
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

		operator Array<Value>() const;

		Set& operator=(const Set&);
		Set& operator=(Set&&) noexcept;

	private:
		struct Node
		{
			uintMem hash;
			Node* next;
			Node* prev;
			Value value;
#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
			uintMem iteratorCount;
#endif

			Node(Node* prev, Node* next, uintMem hash, Value&& value);
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
		void CopyUnsafe(const Set& other);

		static uintMem Hash(const Value& value);

		Bucket* GetBucketFromHash(uintMem hash) const;

		//Wont check if hashMod is smaller than bucketCount
		Bucket* GetBucketFromHashModUnsafe(uintMem hashMod) const;

		Iterator FindWithHint(const Value& value, uintMem hash);
		ConstIterator FindWithHint(const Value& value, uintMem hash) const;

		//Wont check if bucket is nullptr
		Iterator FindWithHintUnsafe(const Value& value, Bucket* bucket);
		ConstIterator FindWithHintUnsafe(const Value& value, Bucket* bucket) const;

		InsertResult InsertWithHint(const Value& value, uintMem hash);

		//Wont check if bucket is nullptr
		InsertResult InsertWithHintUnsafe(const Value& value, Bucket* bucket, uintMem hash);

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
		static void MoveNodeUnsafe(Node* node, Bucket* oldBucket, Bucket* newBucket);
		//Wont check if node or bucket is nullptr or change old node neighbour pointers acordingly
		static void MoveNodeToBeginingUnsafe(Node* node, Bucket* bucket);

		template<typename>
		friend class SetIterator;
	};	
}