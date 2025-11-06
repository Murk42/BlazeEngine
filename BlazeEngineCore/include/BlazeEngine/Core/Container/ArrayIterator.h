#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/Type/TypeTraits.h"

namespace Blaze
{
	/*
		Iterator usage:
		Increasing, decreasing or dereferencing the iterator outside of valid range or invalidating the arrays previous buffer while some
		iterators are referencing it cannot be detected if no debugging measures are taken. It is up to the client to ensure its valid use.

		Null iterators can be created by the default constructor, by copying/assigning an null iterator. They cannot be dereferenced,
		incremented or decremented, it will result in an error or a possible silent fail, depending if the BLAZE_NULL_ITERATOR_CHECK macro
		is defined.

		BLAZE_NULL_ITERATOR_CHECK:
		If BLAZE_NULL_ITERATOR_CHECK is defined, incrementing, decrementing or dereferencing a null array iterator will result in a fatal error.
		Otherwise it will try to continue execution and have a possible silent fail. Having this undefined will slightly improve performance.
		It IS defined in debug mode by default (it isn't defined in release mode).

		No other macros change the array iterator behaviour
	*/
	template<typename T>
	class BLAZE_API ArrayIterator
	{
	public:
		using ValueType = T;
		using StoredType = Conditional<IsReferenceType<T>, RemoveReference<T>* const, T>;

		//STD compatibility
		using value_type = T;
		using difference_type = intMem;

		ArrayIterator();
		ArrayIterator(const ArrayIterator& other);
		/*
			If T is not a reference type this constructor constructs a iterator from a pointer to the object. Otherwise this constructor
			constructs a iterator from a pointer to a pointer to the object. The pointer passed should point to the memory where the the reference is stored.
			The reference should be reinterpret_cast to a pointer and a pointer to that pointer passed to the function. This way the reference is reinterpreted as a pointer which
			can be actually stored and changed.

			\param ptr - pointer to the memory of the reference value.
		*/
		ArrayIterator(StoredType* ptr);
		~ArrayIterator();

		StoredType* Ptr() const;
		bool IsNull() const;

		T& operator*() const;
		StoredType* operator->() const;

		ArrayIterator& operator++();
		ArrayIterator operator++(int);
		ArrayIterator& operator--();
		ArrayIterator operator--(int);

		ArrayIterator operator+(intMem offset) const;
		ArrayIterator operator-(intMem offset) const;

		uintMem operator-(const ArrayIterator& other) const;

		ArrayIterator& operator+=(intMem offset);
		ArrayIterator& operator-=(intMem offset);

		bool operator>(const ArrayIterator& other) const;
		bool operator>=(const ArrayIterator& other) const;
		bool operator<(const ArrayIterator& other) const;
		bool operator<=(const ArrayIterator& other) const;

		bool operator==(const ArrayIterator& other) const;
		bool operator!=(const ArrayIterator& other) const;

		operator ArrayIterator<const T>() const;
		operator const StoredType*() const;
		operator StoredType* () const requires (!IsConstType<T>);

		ArrayIterator& operator=(const ArrayIterator& other);
	private:
		StoredType* ptr;
	};
}

#include "BlazeEngine/Core/Container/ArrayIteratorImpl.h"