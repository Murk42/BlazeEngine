#pragma once
#include "BlazeEngine/DataStructures/TemplateGroup.h"

namespace Blaze
{
	template<typename ... T>
	class Tuple
	{
	public:
		using TemplateGroup = TemplateGroup<T...>;

		constexpr Tuple()
		{

		}
		constexpr Tuple(const Tuple&)
		{

		}
		constexpr Tuple(Tuple&&)
		{

		}

		template<size_t>
		constexpr InvalidType GetValueByIndex()
		{
			return { };
		}

		constexpr Tuple& operator=(const Tuple&) 
		{
			return *this;
		}
		constexpr Tuple& operator=(Tuple&&) noexcept
		{
			return *this;
		}
	};

	template<typename T1>
	class Tuple<T1>
	{
	public:
		using TemplateGroup = Blaze::TemplateGroup<T1>;

		T1 value;		

		constexpr Tuple()
			: value()
		{
		}
		constexpr Tuple(const Tuple& tuple)
			: value(tuple.value)
		{
		}
		constexpr Tuple(Tuple&& tuple) noexcept
			: value(std::move(tuple.value))
		{
		}

		constexpr Tuple(const T1& v1)
			: value(v1)
		{

		}

		template<size_t index>
		constexpr auto& GetValueByIndex()
		{			
			if constexpr (index == 0)
				return value;
			else
				return { };

			static_assert(index == 0, "Index out of bounds");
		}
		template<size_t index>
		constexpr const auto& GetValueByIndex() const
		{
			if constexpr (index == 0)
				return value;
			else
				return { };

			static_assert(index == 0, "Index out of bounds");
		}
		template<typename S>
		constexpr S& GetValueByType()
		{			
			return value;

			static_assert(Template<T1>::template Same<S>, "Invalid type");
		}
		template<typename S>
		constexpr const S& GetValueByType() const
		{
			return value;

			static_assert(Template<T1>::template Same<S>, "Invalid type");
		}

		constexpr Tuple& operator=(const Tuple& tuple)
		{
			value = tuple.value;			
			return *this;
		}
		constexpr Tuple& operator=(Tuple&& tuple) noexcept
		{
			value = std::move(tuple.value);
			return *this;
		}
	};

	template<typename T1, typename ... T>
	class Tuple<T1, T...>
	{
	public:
		using TemplateGroup = Blaze::TemplateGroup<T1, T...>;

		T1 value;
		Tuple<T...> nextTuple;

		constexpr Tuple()
			: value(), nextTuple()
		{
		}
		constexpr Tuple(const Tuple& tuple)
			: value(tuple.value), nextTuple(tuple.nextTuple)
		{ 
		}
		constexpr Tuple(Tuple&& tuple) noexcept
			: value(std::move(tuple.value)), nextTuple(std::move(tuple.nextTuple))
		{
		}

		constexpr Tuple(const T1& v1, const T& ... v)
			: value(v1), nextTuple(v...)
		{

		}

		template<size_t index>
		constexpr auto& GetValueByIndex()
		{
			if constexpr (index == 0)
				return value;
			else
				return nextTuple.GetValueByIndex<index - 1>();

			static_assert(sizeof...(T) > 0, "Index out of bounds");
		}
		template<size_t index>
		constexpr const auto& GetValueByIndex() const
		{
			if constexpr (index == 0)
				return value;
			else
				return nextTuple.GetValueByIndex<index - 1>();

			static_assert(sizeof...(T) > 0, "Index out of bounds");
		}
		template<typename S>
		constexpr S& GetValueByType()
		{
			return GetValueByIndex<TemplateGroup::template IndexOfType<S>>();
		}
		template<typename S>
		constexpr const S& GetValueByType() const
		{
			return GetValueByIndex<TemplateGroup::template IndexOfType<S>>();
		}

		constexpr Tuple& operator=(const Tuple& tuple)
		{
			value = tuple.value;
			nextTuple = tuple.nextTuple;

			return *this;
		}
		constexpr Tuple& operator=(Tuple&& tuple) noexcept
		{
			value = std::move(tuple.value);
			nextTuple = std::move(tuple.nextTuple);

			return *this;
		}
	};

	template<typename G>
	struct _ToTuple
	{
		static_assert(std::_Always_false<G>, "ToTuple requires the first template parameter to be a TemplateGroup");
	};
	template<typename ... G>
	struct _ToTuple<TemplateGroup<G...>>
	{
		using type = Tuple<G...>;
	};

	template<typename G>
	using ToTuple = _ToTuple<G>::template type;
}