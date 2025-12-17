#pragma once
#include "BlazeEngine/Core/Common/Hash.h"
#include "BlazeEngine/Core/Common/Tuple.h"
#include "BlazeEngine/Core/String/String.h"
#include <thread>
#include <string_view>

namespace Blaze
{
	template<>
	class Hash<std::thread::id>
	{
	public:
		using Type = std::thread::id;

		uint64 Compute(const std::thread::id& value) const
		{
			return std::hash<std::thread::id>()(value);
		}
	};
}

namespace std
{
	template <typename Char, Blaze::AllocatorType Allocator>
	struct hash<Blaze::GenericString<Char, Allocator>>
	{
		inline size_t operator()(const Blaze::GenericString<Char, Allocator>& k) const
		{
			return hash<std::basic_string_view<Char>>()(std::basic_string_view<Char>(k.Ptr(), k.Count()));
		}
	};

	template <typename Char>
	struct hash<Blaze::GenericStringView<Char>>
	{
		inline size_t operator()(const Blaze::GenericStringView<Char>& k) const
		{
			return hash<std::basic_string_view<Char>>()(std::basic_string_view<Char>(k.Ptr(), k.Count()));
		}
	};
}

namespace Blaze
{
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