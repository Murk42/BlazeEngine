#pragma once
#include "BlazeEngine/Core/Type/IteratorTraits.h"

namespace Blaze::Algorithm
{


	template<ContainerType C1, ContainerType C2>
	constexpr auto FindRange(const C1& range, const C2& target);

	template<ContainerType C1, ContainerType C2>
	constexpr auto FindRangeReverse(const C1& range, const C2& target);

	template<ContainerType C1, ContainerType C2>
	constexpr bool ContainsRange(const C1& range, const C2& target);

	template<ContainerType C1, ContainerType C2>
	constexpr bool StartsWithRange(const C1& range, const C2& target);

	template<ContainerType C1, ContainerType C2>
	constexpr bool EndsWithRange(const C1& range, const C2& target);

	template<ContainerType C1, ContainerType C2>
	constexpr uintMem CountOfRange(const C1& range, const C2& target);
}

#include "BlazeEngine/Core/Common/AlgorithmImpl.h"