#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/Type/TypeTraitsImpl.h"

namespace Blaze
{
    template<typename T1, typename T2>
    concept SameAs = Internal::SameAs<T1, T2>::value;

	template<typename T1, typename T2>
	concept ConvertibleTo = requires (const T1& value)
	{
		{ static_cast<T2>(value) } -> SameAs<T2>;
	};

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

    template<typename Base, typename Derived>
    concept IsBaseOf = requires(Derived* d) {
        { static_cast<Base*>(d) };
    } && (!SameAs<Base, Derived> || requires { typename Internal::SameAs<Base, Derived>; });

	template<typename Derived, typename Base>
	concept IsDerivedFrom = IsBaseOf<Base, Derived>;


	// Checks if a type is a final class (cannot be inherited from)
	template<typename T>
	concept IsFinal = __is_final(T);

    // Checks if T can be constructed from Args...
    template<typename T, typename... Args>
    concept IsConstructibleFrom = requires(Args&&... args) {
        T{static_cast<Args&&>(args)...};
    };
    template<typename From, typename To>
    concept IsConvertibleTo = requires(From&& from) {
        { static_cast<To>(static_cast<From&&>(from)) } -> SameAs<To>;
    };
}
