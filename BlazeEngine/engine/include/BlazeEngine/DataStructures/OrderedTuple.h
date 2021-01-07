#pragma once
#include "BlazeEngine/DataStructures/TypeTraits.h"

namespace Blaze
{
	template<typename ... T>
	class OrderedTuple;

	template<typename T1, typename T2, typename ... T>
	class OrderedTuple<T1, T2, T...>
	{
	public:
		using Type = T1;
		using NextTuple = OrderedTuple<T2, T...>;
		constexpr static size_t TupleSize = sizeof ... (T) + 2;

		T1 value;
		OrderedTuple<T2, T...> nextTuple;	

		constexpr inline OrderedTuple()
			: value(), nextTuple()
		{
		}

		constexpr inline OrderedTuple(const T1& v1, const T2& v2, const T& ... v)
			: value(v1), nextTuple(v2, v...)
		{

		}

		template<size_t index>
		constexpr at_index_t<index, T1, T2, T...>& GetValue()
		{
			if constexpr (index == 0)
				return value;
			else
				return nextTuple.GetValue<index - 1>();
		}
		template<size_t index>
		constexpr const at_index_t<index, T1, T2, T...>& GetValue() const
		{
			if constexpr (index == 0)
				return value;
			else
				return nextTuple.GetValue<index - 1>();
		}
	};

	template<typename T1>
	class OrderedTuple<T1>
	{
	public:
		using Type = T1;
		using NextTuple = void;
		constexpr static size_t TupleSize = 1;

		T1 value;		

		constexpr inline OrderedTuple()
			: value()
		{

		}
		constexpr inline OrderedTuple(const T1& value)
			: value(value)
		{

		}

		template<size_t index>
		constexpr T1& GetValue()
		{			
			static_assert(index == 0, "index out of range");
			return value;			
		}
		template<size_t index>
		constexpr const T1& GetValue() const
		{
			static_assert(index == 0, "index out of range");
			return value;
		}
	};

}