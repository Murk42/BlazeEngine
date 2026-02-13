#pragma once
#include "BlazeEngine/Core/Debug/Logger.h"

namespace Blaze
{
	template<typename T, RangeIterator Iterator>
	inline constexpr Range<T, Iterator>::Range(Iterator begin, Iterator end)
		: _begin(begin), _end(end)
	{
		if constexpr (RandomAccessIterator<Iterator>)
		{
			if (begin > end)
				BLAZE_LOG_FATAL("Invalid range construction iterators, begin > end is true");
		}
	}

	template<typename T, RangeIterator Iterator>
	inline constexpr Range<T, Iterator>::Range(Iterator begin, uintMem size) requires RandomAccessIterator<Iterator>
		: _begin(begin), _end(begin + size)
	{
	}

	template<typename T, RangeIterator Iterator>
	template<uintMem Size>
	inline constexpr Range<T, Iterator>::Range(T(&arr)[Size]) requires ContiguousIterator<Iterator>
		: _begin(arr), _end(arr + Size)
	{
	}
	template<typename T, RangeIterator Iterator>
	template<ConvertableToRange<T> C>
	inline constexpr Range<T, Iterator>::Range(C& container)
		: Range(begin(container), end(container))
	{

	}
	template<typename T, RangeIterator Iterator>
	template<ConvertableToRange<T> C>
	inline constexpr Range<T, Iterator>::Range(const C& container)
		: Range(begin(container), end(container))
	{

	}
	template<typename T, RangeIterator Iterator>
	inline constexpr bool Range<T, Iterator>::Empty() const
	{
		return _end == _begin;
	}
	template<typename T, RangeIterator Iterator>
	inline constexpr Range<T, Iterator> Range<T, Iterator>::SubRange(intMem start, intMem end) const requires ContiguousIterator<Iterator>
	{
		const uintMem count = Count();

		if (start < 0)
			start += count;

		if (end < 0)
			end += count;

		if (start < 0 || end > static_cast<intMem>(count) || start > end)
			BLAZE_LOG_FATAL("Invalid sub range");

		return Range(_begin + start, _begin + end);
	}
	template<typename T, RangeIterator Iterator>
	inline constexpr uintMem Range<T, Iterator>::Count() const requires ContiguousIterator<Iterator>
	{
		return _end - _begin;
	}
	template<typename T, RangeIterator Iterator>
	inline constexpr T* Range<T, Iterator>::Data() const requires ContiguousIterator<Iterator>
	{
		return &*_begin;
	}
	template<typename T, RangeIterator Iterator>
	inline constexpr T& Range<T, Iterator>::First() const
	{
		if (Empty())
			BLAZE_LOG_FATAL("Accessing an empty range");

		return *_begin;
	}
	template<typename T, RangeIterator Iterator>
	inline constexpr T& Range<T, Iterator>::Last() const requires RandomAccessIterator<Iterator>
	{
		if (Empty())
			BLAZE_LOG_FATAL("Accessing an empty range");

		auto it = _end;
		--it;
		return *it;
	}
	template<typename T, RangeIterator Iterator>
	inline constexpr Iterator Range<T, Iterator>::FirstIterator()
	{
		return _begin;
	}
	template<typename T, RangeIterator Iterator>
	inline constexpr auto Range<T, Iterator>::FirstIterator() const -> ConstIteratorType
	{
		return _begin;
	}
	template<typename T, RangeIterator Iterator>
	inline constexpr Iterator Range<T, Iterator>::LastIterator() requires BidirectionalIterator<Iterator>
	{
		auto it = _end;
		--it;
		return it;
	}
	template<typename T, RangeIterator Iterator>
	inline constexpr auto Range<T, Iterator>::LastIterator() const -> ConstIteratorType requires BidirectionalIterator<Iterator>
	{
		auto it = _end;
		--it;
		return it;
	}
	template<typename T, RangeIterator Iterator>
	inline constexpr Iterator Range<T, Iterator>::AheadIterator() requires BidirectionalIterator<Iterator>
	{
		auto it = _begin;
		--it;
		return it;
	}
	template<typename T, RangeIterator Iterator>
	inline constexpr auto Range<T, Iterator>::AheadIterator() const -> ConstIteratorType requires BidirectionalIterator<Iterator>
	{
		auto it = _begin;
		--it;
		return it;
	}
	template<typename T, RangeIterator Iterator>
	inline constexpr Iterator Range<T, Iterator>::BehindIterator()
	{
		return _end;
	}
	template<typename T, RangeIterator Iterator>
	inline constexpr auto Range<T, Iterator>::BehindIterator() const -> ConstIteratorType
	{
		return _end;
	}
	template<typename T, RangeIterator Iterator>
	inline constexpr T& Range<T, Iterator>::operator[](uintMem index) requires ContiguousIterator<Iterator>
	{
		if (_begin + index >= _end)
			BLAZE_LOG_FATAL("Accessing a range outside its bounds");
		return _begin[index];
	}
	template<typename T, RangeIterator Iterator>
	inline constexpr const T& Range<T, Iterator>::operator[](uintMem index) const requires ContiguousIterator<Iterator>
	{
		if (_begin + index >= _end)
			BLAZE_LOG_FATAL("Accessing a range outside its bounds");
		return _begin[index];
	}


	template<typename T, RangeIterator Iterator>
	constexpr auto begin(Range<T, Iterator>& range)
	{
		return range.FirstIterator();
	}
	template<typename T, RangeIterator Iterator>
	constexpr auto begin(const Range<T, Iterator>& range)
	{
		return range.FirstIterator();
	}
	template<typename T, RangeIterator Iterator>
	constexpr auto end(Range<T, Iterator>& range)
	{
		return range.BehindIterator();
	}
	template<typename T, RangeIterator Iterator>
	constexpr auto end(const Range<T, Iterator>& range)
	{
		return range.BehindIterator();
	}
}