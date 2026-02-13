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
			static_assert(false, "Index out of bounds");
		}
		template<size_t index>
		[[nodiscard]] constexpr const auto& GetValueByIndex() const noexcept
		{
			static_assert(false, "Index out of bounds");
		}
	};

	template<typename... T>
	Tuple(T&&...) -> Tuple<std::decay_t<T>...>;

	template<typename... T>
	struct std::tuple_size<Blaze::Tuple<T...>> : std::integral_constant<size_t, sizeof...(T)> {};

	template<size_t I, typename... T>
	struct std::tuple_element<I, Blaze::Tuple<T...>>
	{
		using type = typename Blaze::TypeGroup<T...>::template TypeAtIndex<I>;
	};

	template <size_t I, typename... T>
	constexpr decltype(auto) get(Blaze::Tuple<T...>& tuple) noexcept
	{
		return tuple.template GetValueByIndex<I>();
	}
	template <size_t I, typename... T>
	constexpr decltype(auto) get(Blaze::Tuple<T...>&& tuple) noexcept
	{
		return tuple.template GetValueByIndex<I>();
	}
	template <size_t I, typename... T>
	constexpr decltype(auto) get(const Blaze::Tuple<T...>& tuple) noexcept
	{
		return tuple.template GetValueByIndex<I>();
	}
	template <typename T, typename... Ts>
	constexpr decltype(auto) get(Blaze::Tuple<Ts...>& tuple) noexcept
	{
		return tuple.template GetValueByType<T>();
	}
	template <typename T, typename... Ts>
	constexpr decltype(auto) get(Blaze::Tuple<Ts...>&& tuple) noexcept
	{
		return tuple.template GetValueByType<T>();
	}
	template <typename T, typename... Ts>
	constexpr decltype(auto) get(const Blaze::Tuple<Ts...>& tuple) noexcept
	{
		return tuple.template GetValueByType<T>();
	}

	template <class _Ty, typename ... T, size_t... _Indices>
	constexpr _Ty _Make_from_tuple_impl(Blaze::Tuple<T...>&& _Tpl, std::index_sequence<_Indices...>)
	{
		return _Ty(get<_Indices>(std::forward<Blaze::Tuple<T...>>(_Tpl))...);
	}

	template <class _Callable, typename ... T, size_t... _Indices>
	constexpr decltype(auto) _Apply_impl(_Callable&& _Obj, Blaze::Tuple<T...>&& _Tpl, std::index_sequence<_Indices...>)
	{
		return std::invoke(std::forward<_Callable>(_Obj), get<_Indices>(std::forward<Blaze::Tuple<T...>>(_Tpl))...);
	}
	template<class _Ty, typename ... T> requires IsConstructibleFrom<_Ty, T...>
	constexpr _Ty make_from_tuple(Blaze::Tuple<T...>&& _Tpl)
	{
		return _Make_from_tuple_impl<_Ty, T...>(std::forward<Blaze::Tuple<T...>>(_Tpl), std::make_index_sequence<sizeof ... (T)>{});
	}
	template <class _Callable, typename ... T>
	constexpr decltype(auto) apply(_Callable&& _Obj, Blaze::Tuple<T...>&& _Tpl)
	{
		return _Apply_impl(_STD forward<_Callable>(_Obj), std::forward<Blaze::Tuple<T...>>(_Tpl), std::make_index_sequence<sizeof...(T)>{});
	}
}

namespace std
{
	template <size_t I, typename... T>
	constexpr decltype(auto) get(Blaze::Tuple<T...>& tuple) noexcept
	{
		return tuple.template GetValueByIndex<I>();
	}
	template <size_t I, typename... T>
	constexpr decltype(auto) get(Blaze::Tuple<T...>&& tuple) noexcept
	{
		return tuple.template GetValueByIndex<I>();
	}
	template <size_t I, typename... T>
	constexpr decltype(auto) get(const Blaze::Tuple<T...>& tuple) noexcept
	{
		return tuple.template GetValueByIndex<I>();
	}
	template <typename T, typename... Ts>
	constexpr decltype(auto) get(Blaze::Tuple<Ts...>& tuple) noexcept
	{
		return tuple.template GetValueByType<T>();
	}
	template <typename T, typename... Ts>
	constexpr decltype(auto) get(Blaze::Tuple<Ts...>&& tuple) noexcept
	{
		return tuple.template GetValueByType<T>();
	}
	template <typename T, typename... Ts>
	constexpr decltype(auto) get(const Blaze::Tuple<Ts...>& tuple) noexcept
	{
		return tuple.template GetValueByType<T>();
	}
}