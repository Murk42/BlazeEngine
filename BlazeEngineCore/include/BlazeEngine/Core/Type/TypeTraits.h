#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/Type/TypeTraitsImpl.h"
#include <type_traits>

namespace Blaze
{
    template<typename T1, typename T2>
    concept SameAs = Internal::SameAs<T1, T2>::value;

	template<bool Condition, typename TrueType, typename FalseType>
	using Conditional = typename Internal::Conditional<Condition, TrueType, FalseType>::Type;

	template<typename T, typename ... Ts>
	concept OneOf = Internal::OneOf<T, Ts...>::value;

	template<typename T>
	using RemoveReference = typename Internal::RemoveReference<T>::Type;
	template<typename T>
	using RemoveConst = typename Internal::RemoveConst<T>::Type;
	template<typename T>
	using RemoveVolatile = typename Internal::RemoveVolatile<T>::Type;

	template<typename T>
	using RemoveCV = typename Internal::RemoveCV<T>::Type;
	template<typename T>
	using RemoveCVRef = RemoveCV<RemoveReference<T>>;

	template<typename T>
	concept IsReferenceType = Internal::IsReferenceType<T>::value;
	template<typename T>
	concept IsConstType = Internal::IsConstType<T>::value;
	template<typename T>
	concept IsVolatileType = Internal::IsVolatileType<T>::value;

	template<typename T>
	concept IsArrayType = Internal::IsArrayType<T>::value;

	template<typename T>
	using ArrayValueType = typename Internal::ArrayValueType<T>::Type;

	template<typename Left, typename Right>
	concept IsAssignableFrom = requires(Left left, Right && right) {
		{ left = static_cast<Right&&>(right) };
	};

#if defined(__clang__) || defined(__GNUC__) || defined(_MSC_VER)
	template<typename Base, typename Derived>
	concept IsBaseOf = __is_base_of(Base, Derived);
	template<typename Derived, typename Base>
	concept IsDerivedFrom = __is_base_of(Base, Derived) && __is_convertible_to(const volatile Derived*, const volatile Base*);
	template<typename T>
	concept IsDestructible = __is_nothrow_destructible(T);
	template<typename T, typename... Args>
	concept IsConstructibleFrom = IsDestructible<T> && __is_constructible(T, Args...);
	template<typename From, typename To>
	concept IsConvertibleTo = __is_convertible_to(From, To) && requires { static_cast<To>(Declval<From>()); };
	template<typename T>
	concept IsFinal = __is_final(T);
	template<typename T, typename ... Args>
	concept IsTriviallyConstructible = __is_trivially_constructible(T, Args...);
	template<typename T>
	concept IsTriviallyDestructible = __is_trivially_destructible(T);
	template<typename T, typename T2>
	concept IsTriviallyAssignable = __is_trivially_assignable(T, T2);
	template<typename T>
	concept HasVirtualDestructor = __has_virtual_destructor(T);
	template<typename T>
	using UnderlyingType = __underlying_type(T);
#else
	// If we reach here, the compiler doesn't expose the usual builtin.
	// We can't reliably reimplement the standard trait in portable, standard C++,
	// so fail loudly so the user can add a compiler-specific branch.

	// (sizeof(T) > 0) is used to keep SFINAE friendliness

	template<typename Base, typename Derived>
	concept IsBaseOf = (sizeof(Base) > 0) && false;
	template<typename Derived, typename Base>
	concept IsDerivedFrom = (sizeof(Base) > 0) && false;
	template<typename T>
	concept IsDestructible = (sizeof(T) > 0) && false;
	template<typename T, typename... Args>
	concept IsConstructibleFrom = (sizeof(T) > 0) && false;
	template<typename From, typename To>
	concept IsConvertibleTo = (sizeof(From) > 0) && false;
	template<typename T>
	concept IsFinal = = (sizeof(T) > 0) && false;
	template<typename T, typename ... Args>
	concept IsTriviallyConstructible = (sizeof(T) > 0) && false;
	template<typename T>
	concept IsTriviallyDestructible = (sizeof(T) > 0) && false;
	template<typename T, typename T2>
	concept IsTriviallyAssignable = (sizeof(T) > 0) && false;
	template<typename T>
	concept HasVirtualDestructor = (sizeof(T) > 0) && false;
	template<typename T>
	using UnderlyingType = (sizeof(T) > 0) && false;;

	static_assert("Your compiler doesn't support type trait builtins. ");
#endif
}
