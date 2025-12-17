#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/Type/TypeGroup.h"

namespace Blaze
{
	template<typename ... T>
	class Tuple;

	template<typename G>
	struct _ToTuple
	{
		static_assert(std::_Always_false<G>, "ToTuple requires the template parameter to be a TypeGroup");
	};
	template<typename ... G>
	struct _ToTuple<TypeGroup<G...>>
	{
		using type = Tuple<G...>;
	};

	template<typename G>
	using ToTuple = typename _ToTuple<G>::type;

	template<typename ... T>
	class Tuple
	{
	public:
		using Types = Blaze::TypeGroup<T...>;
		using FirstType = typename Types::First;
		using NextTypes = typename Types::Next;
		using NextTupleType = ToTuple<NextTypes>;

		[[no_unique_address]] FirstType value;
		[[no_unique_address]] NextTupleType nextTuple;

		constexpr Tuple() = default;
		constexpr Tuple(const Tuple& tuple) = default;
		constexpr Tuple(Tuple&& tuple) noexcept = default;

		template<typename First, typename... Rest> requires (sizeof...(Rest) + 1 == sizeof...(T))
			constexpr Tuple(First&& first, Rest&&... rest)
			: value(std::forward<First>(first)), nextTuple(std::forward<Rest>(rest)...)
		{
		}

		template<size_t index>
		[[nodiscard]] constexpr auto& GetValueByIndex()
		{
			static_assert(index < sizeof...(T), "Index out of bounds");

			if constexpr (index == 0)
				return value;
			else
				return nextTuple.GetValueByIndex<index - 1>();
		}
		template<size_t index>
		[[nodiscard]] constexpr const auto& GetValueByIndex() const
		{
			static_assert(index < sizeof...(T), "Index out of bounds");

			if constexpr (index == 0)
				return value;
			else
				return nextTuple.GetValueByIndex<index - 1>();
		}
		template<typename S>
		[[nodiscard]] constexpr S& GetValueByType() noexcept
		{
			return GetValueByIndex<TypeGroup::template IndexOfType<S>>();
		}
		template<typename S>
		[[nodiscard]] constexpr const S& GetValueByType() const noexcept
		{
			return GetValueByIndex<TypeGroup::template IndexOfType<S>>();
		}

		template<typename F, uintMem indexOffset = 0>
		constexpr void ForEach(F&& func)
		{
			func.template operator() < indexOffset > (value);
			nextTuple.template ForEach<F, indexOffset + 1>(std::forward<F>(func));
		}

		constexpr Tuple& operator=(const Tuple& tuple) = default;
		constexpr Tuple& operator=(Tuple&& tuple) = default;
	private:

	};

	template<>
	class Tuple<>
	{
	public:
		using TypeGroup = TypeGroup<>;

		constexpr Tuple() = default;
		constexpr Tuple(const Tuple&) = default;
		constexpr Tuple(Tuple&&) noexcept = default;

		constexpr Tuple& operator=(const Tuple&) = default;
		constexpr Tuple& operator=(Tuple&&) noexcept = default;

		template<typename F, uintMem indexOffset = 0>
		constexpr void ForEach(F&& func)
		{
			// Do nothing, end of recursion
		}

		template<size_t index>
		[[nodiscard]] constexpr auto& GetValueByIndex() noexcept
		{
			static_assert("Index out of bounds");
		}
		template<size_t index>
		[[nodiscard]] constexpr const auto& GetValueByIndex() const noexcept
		{
			static_assert("Index out of bounds");
		}
	};

	template<typename... T>
	Tuple(T&&...) -> Tuple<std::decay_t<T>...>;


}