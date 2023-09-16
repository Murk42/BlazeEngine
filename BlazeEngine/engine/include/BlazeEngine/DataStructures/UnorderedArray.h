#pragma once

namespace Blaze
{	
	/*
		Unordered unordered array class

		BLAZE_INVALID_ITERATOR_CHECK:
		If BLAZE_INVALID_ITERATOR_CHECK is defined, passing an invalid iterator or index to a unordered array will result in a fatal error. Having this
		undefined will slightly improve performance. It IS defined in debug mode by default (not defined in release mode).

		BLAZE_CONTAINER_INVALIDATION_CHECK:
		By changing a unordered array that an unordered array iterator is pointing to (adding, erasing, appending, resizing, clearing, etc...) and therefore possibly
		changing the unordered arrays buffer location, the iterator becomes invalid. If BLAZE_CONTAINER_INVALIDATION_CHECK is defined and any operation
		noted above is executed and there is a unordered array iterator referencing the unordered array, a warning will be logged. Otherwise it will try to continue
		execution and have a possible silent fail. When this macro is defined, the size of the unordered array iterator and the unordered array itself change because
		additional information is needed to be stored for this macro to work. Having this undefined will improve performance. It is NOT defined
		in debug mode by default (not defined in release mode either).

		No other macros change the unordered array behaviour
	*/
	template<typename T, AllocatorType Allocator = Blaze::DefaultAllocator>
	class BLAZE_API UnorderedArray
	{
	public:
		using Iterator = ArrayIterator<UnorderedArray>;
		using ConstIterator = ArrayIterator<const UnorderedArray>;
		using ValueType = T;
		using value_type = ValueType;

		UnorderedArray();
		UnorderedArray(const UnorderedArray& arr);
		UnorderedArray(UnorderedArray&& arr) noexcept;

		template<typename ... Args> requires std::constructible_from<T, Args...>
		UnorderedArray(uint count, const Args& ... args);
		template<uint S>
		UnorderedArray(const T(&arr)[S]);
		UnorderedArray(const T* ptr, uint count);
		UnorderedArray(const std::initializer_list<T>& arr);
		UnorderedArray(const ArrayView<std::remove_const_t<T>>& arr);
		UnorderedArray(const ArrayView<const std::remove_const_t<T>>& arr);

		~UnorderedArray();

		void Clear();
		bool Empty() const;
		uint Count() const;
		
		template<typename ... Args> requires std::constructible_from<T, Args...>
		Iterator Add(Args&& ... args);
		
		void EraseAt(uint index);
		void EraseAt(Iterator it);

		void Append(const UnorderedArray& other);
		void Append(UnorderedArray&& other);

		template<typename ... Args> requires std::constructible_from<T, Args...>
		void Resize(uint newCount, const Args& ... args);

		T& operator[](uint i);
		const T& operator[](uint i) const;

		T* Ptr();
		const T* Ptr() const;

		T& First();
		const T& First() const;
		T& Last();
		const T& Last() const;

		Iterator GetIterator(uint index);
		ConstIterator GetIterator(uint index) const;

		/*
			Returns an iterator pointing to the first element in the unordered array. If the unordered array is empty returns a null unordered array iterator.
		*/
		Iterator FirstIterator();
		/*
			Returns an iterator pointing to the first element in the unordered array. If the unordered array is empty returns a null unordered array iterator.
		*/
		ConstIterator FirstIterator() const;
		/*
			Returns an iterator pointing to the last element in the unordered array. If the unordered array is empty returns a null unordered array iterator.
		*/
		Iterator LastIterator();
		/*
			Returns an iterator pointing to the last element in the unordered array. If the unordered array is empty returns a null unordered array iterator.
		*/
		ConstIterator LastIterator() const;
		/*
			Returns an iterator pointing one element behind the unordered array. If the unordered array is empty returns a null unordered array iterator.
		*/
		Iterator AheadIterator();
		/*
			Returns an iterator pointing one element ahead the unordered array. If the unordered array is empty returns a null unordered array iterator.
		*/
		ConstIterator AheadIterator() const;
		/*
			Returns an iterator pointing one element behind the unordered array. If the unordered array is empty returns a null unordered array iterator.
		*/
		Iterator BehindIterator();
		/*
			Returns an iterator pointing one element behind the unordered array. If the unordered array is empty returns a null unordered array iterator.
		*/
		ConstIterator BehindIterator() const;

		operator ArrayView<std::remove_const_t<T>>();
		operator ArrayView<const std::remove_const_t<T>>();

		UnorderedArray& operator=(const ArrayView<std::remove_const_t<T>>& other);
		UnorderedArray& operator=(const ArrayView<const std::remove_const_t<T>>& other);
		UnorderedArray& operator=(const UnorderedArray& other);
		UnorderedArray& operator=(UnorderedArray&& other) noexcept;

		template<typename>
		friend class ArrayIterator;
	private:
		T* ptr;
		uint count;
		uint reserved;
		BLAZE_ALLOCATOR_ATTRIBUTE Allocator allocator;

#ifdef BLAZE_CONTAINER_INVALIDATION_CHECK
		uint iteratorCount;
#endif

		/*

			Copies <count> elements starting at <src> without destroying previous
			elements. Changes the current buffer, so the old buffer is freed and a
			new one allocated if needed.

		\param
			src - start of the source unordered array to copy from
		\param
			count - number of elements to copy

		*/
		void CopyUnsafe(const T* src, uint count);

		/*

			Allocates a new buffer if the old one needs to be changed, returns nullptr
			otherwise. Only changes the 'reserved' variable. Doesn't move, copy or
			destroy anything in the old or the new buffer, just allocates the memory.
			Doesn't free the old buffer.

		\param
			newCount - number of elements that will be able to fit in the new buffer

		\returns
			A pointer to a new buffer if the old one needs to be changed. Nullptr otherwise.

		*/
		T* ReallocateUnsafe(uint newCount);
	};

}