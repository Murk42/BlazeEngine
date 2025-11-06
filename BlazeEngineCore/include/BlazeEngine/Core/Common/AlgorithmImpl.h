#pragma once

namespace Blaze::Algorithm
{
	template<ContainerType C1, ContainerType C2>
	constexpr auto FindRange(const C1& range, const C2& target)
	{
		if constexpr (ContiguousContainer<C1> && ContiguousContainer<C2>)
			if (end(range) - begin(range) < end(target) - begin(target))
				return end(range);

		for (auto i = begin(range); i != end(range); ++i)
		{
			bool found = true;

			auto c = i;
			for (auto j = begin(target); j != end(target) && c != end(range); ++j, ++c)
				if (*c != *j)
				{
					found = false;
					break;
				}

			if (found)
				return i;
		}

		return end(range);
	}
	template<ContainerType C1, ContainerType C2>
	constexpr auto FindRangeReverse(const C1& range, const C2& target)
	{
		auto i = end(range);

		if constexpr (ContiguousContainer<C1> && ContiguousContainer<C2>)
		{
			if (end(range) - begin(range) < end(target) - begin(target))
				return end(range);

			i -= end(target) - begin(target);
		}
		else
		{
			for (auto j = begin(target); j != end(target); ++j)
				--i;
		}

		while (true)
		{
			bool found = true;

			auto c = i;
			for (auto j = begin(target); j != end(target) && c != end(range); ++j, ++c)
				if (*c != *j)
				{
					found = false;
					break;
				}

			if (found)
				return i;


			if (i == begin(range))
				break;

			--i;
		}

		return end(range);
	}
	template<ContainerType C1, ContainerType C2>
	constexpr bool ContainsRange(const C1& range, const C2& target)
	{
		return FindRange(range, target) != end(range);
	}

	template<ContainerType C1, ContainerType C2>
	constexpr bool StartsWithRange(const C1& range, const C2& target)
	{
		auto it1 = begin(range);
		auto it2 = begin(target);
		for (; it2 != end(target); ++it1, ++it2)
		{
			if (it1 == end(range) || *it1 != *it2)
				return false;
		}
		return true;
	}

	template<ContainerType C1, ContainerType C2>
	constexpr bool EndsWithRange(const C1& range, const C2& target)
	{
		if constexpr (ContiguousContainer<C1> && ContiguousContainer<C2>)
		{
			if (end(range) - begin(range) < end(target) - begin(target))
				return false;
			auto it1 = end(range) - (end(target) - begin(target));
			auto it2 = begin(target);
			for (; it2 != end(target); ++it1, ++it2)
			{
				if (*it1 != *it2)
					return false;
			}
			return true;
		}
		else if constexpr (BidirectionalContainer<C1>)
		{
			auto it1 = end(range);
			auto it2 = end(range);
			for (; it2 != begin(target); --it1, --it2)
			{
				if (it1 == begin(range) || *it1 != *it2)
					return false;
			}
			return true;
		}
		else
		{
			static_assert(false, "Not Implemented");
		}
	}

	template<ContainerType C1, ContainerType C2>
	constexpr uintMem CountOfRange(const C1& range, const C2& target)
	{
		uintMem count = 0;
		auto it = begin(range);
		while (it != end(range))
		{
            // In-place implementation of FindRange for CountOfRange
            auto subrange_begin = it;
            auto subrange_end = end(range);
            auto target_begin = begin(target);
            auto target_end = end(target);

            auto found = subrange_end;
			for (auto i = subrange_begin; i != subrange_end; ++i)
			{
				bool match = true;
				auto c = i;
				for (auto j = target_begin; j != target_end && c != subrange_end; ++j, ++c)
				{
					if (*c != *j)
					{
						match = false;
						break;
					}
				}
				if (match)
				{
					found = i;
					break;
				}
			}
            it = found;

			if (it != end(range))
			{
				++count;
				++it;
			}
		}
		return count;
	}

}