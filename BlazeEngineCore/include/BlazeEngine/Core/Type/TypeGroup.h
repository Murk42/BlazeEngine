#pragma once
namespace Blaze
{
	template<typename ... T>
	class TypeGroup
	{
	public:
		using First = InvalidType;
		static constexpr size_t Size = 0;

		static constexpr bool AllTypesDifferent = true;
		template<template<typename> class C>
		static constexpr bool AppliesToAll = true;
	};

	template<typename T1, typename ... T>
	class TypeGroup<T1, T...>
	{
		template<size_t index, typename ... T>
		struct _TypeAtIndex
		{
			static_assert("TypeGroup type index out of bounds");

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
			static_assert("Cant get the index of the type if it isnt in the TypeGroup");

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
			static constexpr bool value = C<T1>::value && _AppliesToAll<C, T...>::value;
		};

		template<typename T>
		struct _Join
		{
		};
		template<typename ... Ts2>
		struct _Join<TypeGroup<Ts2...>>
		{
			using type = TypeGroup<T1, T..., Ts2...>;
		};
	public:
		using First = T1;
		using Next = TypeGroup<T...>;
		static constexpr size_t Size = sizeof...(T) + 1;

		template<size_t index>
		using TypeAtIndex = typename _TypeAtIndex<index, T1, T...>::template type;
		template<typename ... O>
		using Add = TypeGroup<T1, T..., O...>;
		template<typename T>
		using Join = _Join<T>::type;
		template<typename S>
		static constexpr size_t IndexOfType = _IndexOfType<0, S, T1, T...>::index;
		template<typename S>
		static constexpr bool HasType = _HasType<S, T1, T...>::value;
		template<typename ... S>
		static constexpr bool HasTypes = (_HasType<S, T1, T...>::value && ...);
		template<template<typename> class C>
		static constexpr bool AppliesToAll = _AppliesToAll<C, T1, T...>::value;
	};
}