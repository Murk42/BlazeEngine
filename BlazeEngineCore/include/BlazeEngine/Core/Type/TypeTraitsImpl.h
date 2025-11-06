#pragma once

namespace Blaze
{
	namespace Internal
	{
		template<typename T1, typename T2>
		struct SameAs
		{
			static constexpr bool value = false;
		};
		template<typename T>
		struct SameAs<T, T>
		{
			static constexpr bool value = true;
		};

		template<bool Value, typename A, typename B>
		struct Conditional;
		template<typename A, typename B>
		struct Conditional<true, A, B>
		{
			using Type = A;
		};
		template<typename A, typename B>
		struct Conditional<false, A, B>
		{
			using Type = B;
		};

		template<typename T, typename ... Ts>
		struct OneOf
		{
			static constexpr bool value = false;
		};
		template<typename T, typename ... Ts>
		struct OneOf<T, T, Ts...>
		{
			static constexpr bool value = true;
		};
		template<typename T, typename U, typename ... Ts>
		struct OneOf<T, U, Ts...>
		{
			static constexpr bool value = OneOf<T, Ts...>::value;
		};

		template<typename T>
		struct RemoveReference
		{
			using Type = T;
		};
		template<typename T>
		struct RemoveReference<T&>
		{
			using Type = T;
		};

		template<typename T>
		struct RemoveConst
		{
			using Type = T;
		};
		template<typename T>
		struct RemoveConst<const T>
		{
			using Type = T;
		};

		template<typename T>
		struct RemoveVolatile
		{
			using Type = T;
		};
		template<typename T>
		struct RemoveVolatile<volatile T>
		{
			using Type = T;
		};

		template<typename T>
		struct RemoveCV
		{
			using Type = T;
		};
		template<typename T>
		struct RemoveCV<const T>
		{
			using Type = T;
		};
		template<typename T>
		struct RemoveCV<volatile T>
		{
			using Type = T;
		};
		template<typename T>
		struct RemoveCV<const volatile T>
		{
			using Type = T;
		};

		template<typename T>
		struct IsReferenceType
		{
			static constexpr bool value = false;
		};
		template<typename T>
		struct IsReferenceType<T&>
		{
			static constexpr bool value = true;
		};

		template<typename T>
		struct IsConstType
		{
			static constexpr bool value = false;
		};
		template<typename T>
		struct IsConstType<const T>
		{
			static constexpr bool value = true;
		};

		template<typename T>
		struct IsVolatileType
		{
			static constexpr bool value = false;
		};
		template<typename T>
		struct IsVolatileType<volatile T>
		{
			static constexpr bool value = true;
		};

		template<typename T>
		struct IsArrayType
		{
			static constexpr bool value = false;
		};
		template<typename T, uintMem Size>
		struct IsArrayType<T[Size]>
		{
			static constexpr bool value = true;
		};
		template<typename T>
		struct IsArrayType<T[]>
		{
			static constexpr bool value = true;
		};
		template<typename T>
		struct ArrayValueType
		{

		};
		template<typename T, uintMem Size>
		struct ArrayValueType<T[Size]>
		{
			using Type = T;
		};
		template<typename T>
		struct ArrayValueType<T[]>
		{
			using Type = T;
		};
	}
}