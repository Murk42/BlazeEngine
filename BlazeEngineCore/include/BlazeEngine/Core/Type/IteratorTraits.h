#pragma once
#include "BlazeEngine/Core/Type/TypeTraits.h"

#include <iterator>

namespace Blaze
{
	template <typename Container>
	concept ContainerType =
		(requires(const Container & c) {
			{ begin(c) };
			{ end(c) };
	} || requires(const Container & c) {
		{ std::begin(c) };
		{ std::end(c) };
	})
	&& requires(decltype(begin(Declval<const Container&>())) it) {
		++it;
		*it;
		it == it;
		static_cast<bool>(it == it);
	};

	template<typename Container, typename T>
	concept ContainerTypeOf = ContainerType<Container> &&
		requires(const Container & c) {
		static_cast<const T&>(*begin(c));
	};

	template<typename Iterator>
	concept InputOutputIterator =
		requires(Iterator it) {
			{ ++it } -> SameAs<Iterator&>;
			*it;
			it == it;
			static_cast<bool>(it == it);
	};

	template<typename Iterator>
	concept BidirectionalIterator = InputOutputIterator<Iterator> &&
		requires(Iterator it) {
			{ --it } -> SameAs<Iterator&>;
	};

	template<typename Iterator>
	concept RandomAccessIterator =
		requires(Iterator it, uintMem n) {
			{ it += n } -> SameAs<Iterator&>;
			{ it - it } -> ConvertibleTo<uintMem>;
			{ it[n] };
	};

	template<typename Iterator>
	concept ContiguousIterator = RandomAccessIterator<Iterator> && requires (Iterator it) {
		static_cast<uintMem>(it + 1 - it);
	};

	template<typename Container>
	using ContainerIteratorType = decltype(begin(Declval<Container&>()));

	template<typename Container>
	using ContainerValueType = decltype(*Declval<ContainerIteratorType<Container>&>());

	template<typename Container>
	concept BidirectionalContainer = BidirectionalIterator<ContainerIteratorType<Container>>;

	template<typename Container>
	concept RandomAccessContainer = RandomAccessIterator<ContainerIteratorType<Container>>;

	template<typename Container>
	concept ContiguousContainer = ContiguousIterator<ContainerIteratorType<Container>>;
}