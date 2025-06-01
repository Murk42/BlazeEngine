#pragma once
#include <concepts>

namespace Blaze
{
	template<typename T>
	concept STDHashable = requires(const T & x)
	{
		{ std::hash<T>{ }(x) } -> std::same_as<size_t>;
	};

	template<typename T>
	concept BlazeHashable = requires(const T & x)
	{
		{ x.Hash() } -> std::same_as<uint32>;
	};

	template<typename T>
	concept Hashable = BlazeHashable<T> || STDHashable<T>;		

	template<typename T>
	concept HashableType = Hashable<T>;

	template<HashableType T>
	class Hash
	{		
	public:
		using Type = T;

		constexpr static uint32 Compute(const T& value)
		{
			return ComputeSpecific(value);
		}
	private:		

		template<typename T> requires BlazeHashable<T> && (!STDHashable<T>)
		constexpr static uint32 ComputeSpecific(const T& value)
		{
			return value.Hash();
		}

		template<typename T> requires (!BlazeHashable<T>) && STDHashable<T>
		constexpr static uint32 ComputeSpecific(const T& value)
		{
			return static_cast<uint32>(std::hash<T>{ }(value));
		}

		template<typename T> requires BlazeHashable<T> && STDHashable<T>
		constexpr static uint32 ComputeSpecific(const T& value)
		{
			return value.Hash();
		}
	};	

	template<typename HasherType>
	struct _HasherType : std::false_type { };
	template<HashableType T>
	struct _HasherType<Hash<T>> : std::true_type { };

	template<typename T>
	concept IsHasher = _HasherType<T>::value;
	template<typename T>
	concept HasherType = IsHasher<T>;

	template<typename Hasher, typename Type>
	concept HasherOf = IsHasher<Hasher> && std::same_as<Type, typename Hasher::template Type>;
}