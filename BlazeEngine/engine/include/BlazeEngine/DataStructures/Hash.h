#pragma once

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

	template<Hashable T>
	class Hash
	{		
	public:
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
			return std::hash<T>{ }(value);
		}

		template<typename T> requires BlazeHashable<T> && STDHashable<T>
		constexpr static uint32 ComputeSpecific(const T& value)
		{
			return value.Hash();
		}
	};	
}