#pragma once
#include "BlazeEngine/DataStructures/Template.h"
#include <type_traits>

namespace Blaze
{
	template<typename ... T>
	class TemplateGroup
	{
	public:
		static constexpr size_t Size = 0;

		static constexpr bool AllTypesDifferent = true;
		template<template<typename> class C>
		static constexpr bool AppliesToAll = true;
	};

	template<typename T1, typename ... T>
	class TemplateGroup<T1, T...>
	{
		template<size_t index, typename ... T>
		struct _TypeAtIndex
		{
			static_assert(std::_Always_false<std::integral_constant<size_t, index>>, "TemplateGroup type index out of bounds");

			using type = InvalidType;
		};
		template<typename T1, typename ... T>
		struct _TypeAtIndex<0, T1, T...>
		{
			using type = T1;
		};
		template<size_t index, typename T1, typename ... T>
		struct _TypeAtIndex<index, T1, T...>
		{
			using type = typename _TypeAtIndex<index - 1, T...>::type;
		};

		template<size_t index, typename S, typename ... T>
		struct _IndexOfType
		{
			static_assert(std::_Always_false<std::integral_constant<size_t, index>>, "Cant get the index of the type if it isnt in the TemplateGroup");

			static constexpr size_t index = -1;
		};
		template<size_t index, typename S, typename T1, typename ... T>
		struct _IndexOfType<index, S, T1, T...>
		{
			static constexpr size_t index = _IndexOfType<index + 1, S, T...>::index;
		};
		template<size_t i, typename T1, typename ... T>
		struct _IndexOfType<i, T1, T1, T...>
		{
			static constexpr size_t index = i;
		};

		template<typename S, typename ... T>
		struct _HasType
		{
			static constexpr bool value = false;
		};
		template<typename S, typename ... T>
		struct _HasType<S, S, T...>
		{
			static constexpr bool value = true;
		};
		template<typename S, typename T1, typename ... T>
		struct _HasType<S, T1, T...>
		{
			static constexpr bool value = _HasType<S, T...>::value;
		};

		template<template<typename> class C, typename ... T>
		struct _AppliesToAll
		{
			static constexpr bool value = true;
		};
		template<template<typename> class C, typename T1, typename ... T>
		struct _AppliesToAll<C, T1, T...>
		{
			static constexpr bool value = C<T1>::value && _AppliesToAll<C, T...>::template value;
		};

		template<typename ... T>
		struct _AllTypesDifferent
		{
			static constexpr bool value = true;
		};
		template<typename T1, typename T2>
		struct _AllTypesDifferent<T1, T2>
		{
			static constexpr bool value = !Template<T1>::template Same<T2>;
		};
		template<typename T1, typename T2, typename ... T>
		struct _AllTypesDifferent<T1, T2, T...>
		{
			static constexpr bool value = !Template<T1>::template Same<T2> && _AllTypesDifferent<T1, T...>::value;
		};

	public:
		using First = T1;
		using Next = TemplateGroup<T...>;
		static constexpr size_t Size = sizeof...(T) + 1;

		template<size_t index>
		using TypeAtIndex = typename _TypeAtIndex<index, T1, T...>::template type;
		template<typename ... O>
		using Add = TemplateGroup<T1, T..., O...>;
		template<typename S>
		static constexpr size_t IndexOfType = _IndexOfType<0, S, T1, T...>::index;
		template<typename S>
		static constexpr bool HasType = _HasType<S, T1, T...>::value;
		template<typename ... S>
		static constexpr bool HasTypes = (_HasType<S, T1, T...>::value && ...);
		template<template<typename> class C>
		static constexpr bool AppliesToAll = _AppliesToAll<C, T1, T...>::value;

		static constexpr bool AllTypesDifferent = _AllTypesDifferent<T1, T...>::value && TemplateGroup<T...>::AllTypesDifferent;
	};

	template<typename T>
	struct _IsTemplateGroup
	{
		static constexpr bool value = false;
	};
	template<typename ... T>
	struct _IsTemplateGroup<TemplateGroup<T...>>
	{
		static constexpr bool value = true;
	};

	template<typename T>
	constexpr bool IsTemplateGroup = _IsTemplateGroup<T>::value;
}