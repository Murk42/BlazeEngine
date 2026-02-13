#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/Type/IntegerTraits.h"

namespace Blaze
{
	template<typename T>
	class Hash;

	template<typename T>
	concept Hashable = requires(Hash<T> hasher, const T & value)
	{
		{ hasher.Compute(value) } -> SameAs<uint64>;
	} && SameAs<typename Hash<T>::template Type, T>;

	template<typename T>
	concept HashableType = Hashable<T>;

	namespace Internal
	{
		template<typename HasherType>
		struct _HasherType { static constexpr bool value = false; };
		template<Hashable T>
		struct _HasherType<Hash<T>> { static constexpr bool value = true; };
	}

	template<typename T>
	concept IsHasher = Internal::_HasherType<T>::value;
	template<typename T>
	concept HasherType = IsHasher<T>;

	template<typename Hasher, typename Type>
	concept HasherOf = IsHasher<Hasher> && SameAs<Type, typename Hasher::template Type>;

	template<typename T>
	concept HasHashFunction = requires(const T & value)
	{
		{ value.Hash() } -> SameAs<uint64>;
	};

	template<HasHashFunction T>
	class Hash<T>
	{
	public:
		using Type = T;

		uint64 Compute(T const& value) const
		{
			return value.Hash();
		}
	};

	template<typename T>
	class Hash<T*>
	{
	public:
		using Type = T*;

		uint64 Compute(T* const& value) const
		{
			uint64 valueInt = (uint64)value;
			valueInt ^= valueInt >> 33;
			valueInt *= 0xff51afd7ed558ccdULL;
			valueInt ^= valueInt >> 33;
			valueInt *= 0xc4ceb9fe1a85ec53ULL;
			valueInt ^= valueInt >> 33;
			return valueInt;
		}
	};

	template<>
	class Hash<uint64>
	{
	public:
		using Type = uint64;

		inline uint64 Compute(uint64 x) const
		{
			x ^= x >> 33;
			x *= 0xff51afd7ed558ccdULL;
			x ^= x >> 33;
			x *= 0xc4ceb9fe1a85ec53ULL;
			x ^= x >> 33;
			return x;
		}
	};

	template<IntegerType T>
	class Hash<T>
	{
	public:
		using Type = T;

		inline uint64 Compute(const T& x) const
		{
			return Hash<uint64>().Compute(static_cast<uint64>(x));
		}
	};

	template<DecimalType T> requires (sizeof(T) <= 8)
	class Hash<T>
	{
	public:
		using Type = T;

		inline uint64 Compute(T x) const
		{
			if (x == static_cast<T>(0))
				x = static_cast<T>(0); // maps -0.0 to +0.0

			uint64 bits = 0;
			*reinterpret_cast<T*>(&bits) = x;
			return Hash<uint64>().Compute(bits);
		}
	};
}