#pragma once
#include <type_traits>
#include <tuple>

namespace Blaze
{
	template<template<typename> class C, typename ... T>
	constexpr unsigned check_count_v = (C<T>::value + ...);

	template <typename, typename... >
	struct index_of;

	template <typename T, typename... R>
	struct index_of<T, T, R...>
	{
		static constexpr size_t value = 0;
	};

	template <typename T, typename F, typename... R>
	struct index_of<T, F, R...>
	{
		static constexpr size_t value = 1 + index_of<T, R...>::value;
	};

	template<typename T1, typename ... T>
	constexpr size_t index_of_v = index_of<T1, T...>::value;
	
	template<size_t index, typename ... T>
	struct at_index
	{		
		using type = std::tuple_element_t<index, std::tuple<T...>>;
	};
	
	template<size_t index, typename ... T>
	using at_index_t = typename at_index<index, T...>::type;
}