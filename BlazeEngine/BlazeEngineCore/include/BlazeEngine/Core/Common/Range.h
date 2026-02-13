#pragma once
#include "BlazeEngine/Core/Type/TypeTraits.h"

namespace Blaze
{
	template<typename Iterator>
	concept RangeIterator = InputOutputIterator<Iterator>;

	template<typename Iterator, typename T>
	concept RangeIteratorOf = RangeIterator<Iterator> &&
		requires(const Iterator it) { { *it } -> SameAs<T&>; } ||
		requires(const Iterator it) { { *it } -> SameAs<RemoveConst<T>&>; };

	template<typename Container, typename T>
	concept ConvertableToRange =
		requires(Container x) {
			{ begin(x) } -> RangeIteratorOf<T>;
			{ end(x) } -> RangeIteratorOf<T>;
		};

	template<typename T, RangeIterator Iterator = T*>
	class Range
	{
	public:
		using ValueType = T;
		using IteratorType = Iterator;
		using ConstIteratorType = Iterator;

		constexpr Range() = default;
		constexpr Range(const Range&) = default;
		constexpr Range(Iterator begin, Iterator end);
		constexpr Range(Iterator begin, uintMem size) requires RandomAccessIterator<Iterator>;
		template<uintMem Size>
		constexpr Range(T(&arr)[Size]) requires ContiguousIterator<Iterator>;
		template<ConvertableToRange<T> C>
		constexpr Range(C& container);
		template<ConvertableToRange<T> C>
		constexpr Range(const C& container);

		constexpr bool Empty() const;

		constexpr Range<T, Iterator> SubRange(intMem start, intMem end) const requires ContiguousIterator<Iterator>;

		constexpr uintMem Count() const requires ContiguousIterator<Iterator>;
		constexpr T* Data() const requires ContiguousIterator<Iterator>;
		constexpr T& First() const;
		constexpr T& Last() const requires RandomAccessIterator<Iterator>;

		constexpr Iterator FirstIterator();
		constexpr ConstIteratorType FirstIterator() const;
		constexpr Iterator LastIterator() requires BidirectionalIterator<Iterator>;
		constexpr ConstIteratorType LastIterator() const requires BidirectionalIterator<Iterator>;
		constexpr Iterator AheadIterator() requires BidirectionalIterator<Iterator>;
		constexpr ConstIteratorType AheadIterator() const requires BidirectionalIterator<Iterator>;
		constexpr Iterator BehindIterator();
		constexpr ConstIteratorType BehindIterator() const;

		constexpr T& operator[](uintMem index) requires ContiguousIterator<Iterator>;
		constexpr const T& operator[](uintMem index) const requires ContiguousIterator<Iterator>;


		constexpr Range& operator=(const Range&) = default;
	private:
		Iterator _begin;
		Iterator _end;
	};

	template<typename Container>
	Range(Container&) -> Range<RemoveReference<decltype(*begin(std::declval<Container&>()))>, decltype(begin(std::declval<Container&>()))>;
	template<typename Container>
	Range(const Container&) -> Range<RemoveReference<decltype(*begin(std::declval<const Container&>()))>, decltype(begin(std::declval<const Container&>()))>;

	template<typename T>
	Range(T*, T*) -> Range<T, T*>;
	template<RangeIterator T>
	Range(T, T) -> Range<decltype(*std::declval<T>), T>;
}

#include "BlazeEngine/Core/Common/RangeImpl.h"