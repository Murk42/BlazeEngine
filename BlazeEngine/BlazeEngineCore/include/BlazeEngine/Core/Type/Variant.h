#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/Type/TypeTraits.h"

namespace Blaze
{
	template<typename ... Ts>
	class Variant
	{
	public:
		using Types = TypeGroup<Ts...>;
		using ValueType = FittedUint<sizeof ... (Ts) + 1>;

		Variant();
		Variant(const Variant& other) requires (IsConstructibleFrom<Ts, const Ts&> && ...);
		Variant(Variant&& other) noexcept requires (IsConstructibleFrom<Ts, Ts&&> && ...);
		template<OneOf<Ts...> T>
		Variant(const T& value) requires IsConstructibleFrom<T, const T&>;
		template<OneOf<Ts...> T>
		Variant(T&& value) requires IsConstructibleFrom<T, T&&>;
		~Variant();

		ValueType GetValueType() const;
		template<OneOf<Ts...> T> 
		bool IsType() const;

		template<OneOf<Ts...> T>
		bool TryGetValue(T& value) const;
		template<typename Callable> requires (std::invocable<Callable, const Ts&> || ...)
		bool TryProcess(const Callable& callable) const;

		template<OneOf<Ts...> T>
		static constexpr ValueType GetValueTypeOf();
		template<typename T>
		static constexpr bool IsTypeValid();

		template<typename ... Ts2>
		bool TryConvert(Variant<Ts2...>& other);

		Variant& operator=(const Variant& other) requires (IsConstructibleFrom<Ts, const Ts&> && ...);
		Variant& operator=(Variant&& other) noexcept requires (IsConstructibleFrom<Ts, Ts&&> && ...);
		template<OneOf<Ts...> T>
		Variant& operator=(const T& value) requires IsConstructibleFrom<T, const T&>;
		template<OneOf<Ts...> T>
		Variant& operator=(T&& value) noexcept requires IsConstructibleFrom<T, T&&>;
	private:
		ValueType type;
		alignas(Ts...) uint8 buffer[std::max({ sizeof(Ts)... })];

		template<OneOf<Ts...> T>
		constexpr T& GetValueUnsafe();
		template<OneOf<Ts...> T>
		constexpr const T& GetValueUnsafe() const;

		constexpr void Destruct();

		template<OneOf<Ts...> T>
		static constexpr void CopyConstructSpecific(Variant& object, const Variant& other);
		template<OneOf<Ts...> T>
		static constexpr void MoveConstructSpecific(Variant& object, Variant&& other);
		template<OneOf<Ts...> T>
		static constexpr void DestructSpecific(Variant& object);
		template<OneOf<Ts...> T>
		static constexpr void CopyAssignSpecific(Variant& object, const Variant& other);
		template<OneOf<Ts...> T>
		static constexpr void MoveAssignSpecific(Variant& object, Variant&& other);

		template<typename ... Args>
		static constexpr void Apply(ValueType type, void(*functions[])(Args&&...), Args&& ... args);
	};
	template<typename ...Ts>
	inline Variant<Ts...>::Variant()
		: type(static_cast<ValueType>(sizeof...(Ts)))
	{
	}
	template<typename ...Ts>
	inline Variant<Ts...>::Variant(const Variant& other) requires (IsConstructibleFrom<Ts, const Ts&> && ...)
		: type(other.type)
	{
		void(*functions[])(Variant&, const Variant&) = { &CopyConstructSpecific<Ts>... };
		Apply<Variant&, const Variant&>(type, functions, *this, other);
	}
	template<typename ...Ts>
	inline Variant<Ts...>::Variant(Variant&& other) noexcept requires (IsConstructibleFrom<Ts, Ts&&> && ...)
		: type(other.type)
	{
		void(*functions[])(Variant&, Variant&&) = { &MoveConstructSpecific<Ts>... };
		Apply<Variant&, Variant&&>(type, functions, *this, std::move(other));
	}
	template<typename ...Ts>
	template<OneOf<Ts...> T>
	inline Variant<Ts...>::Variant(const T& value) requires IsConstructibleFrom<T, const T&>
		: type(GetValueTypeOf<T>())
	{
		new (&GetValueUnsafe<T>()) T(value);
	}
	template<typename ...Ts>
	template<OneOf<Ts...> T>
	inline Variant<Ts...>::Variant(T&& value) requires IsConstructibleFrom<T, T&&>
		: type(GetValueTypeOf<T>())
	{
		new (&GetValueUnsafe<T>()) T(std::forward<T>(value));
	}
	template<typename ...Ts>
	inline Variant<Ts...>::~Variant()
	{
		Destruct();
	}
	template<typename ...Ts>
	template<OneOf<Ts...> T>
	inline bool Variant<Ts...>::TryGetValue(T& value) const
	{
		if (type != GetValueTypeOf<T>())
			return false;

		value = GetValueUnsafe<T>();

		return true;
	}
	template<typename ...Ts>
	template<typename Callable> requires (std::invocable<Callable, const Ts&> || ...)
	inline bool Variant<Ts...>::TryProcess(const Callable& callable) const
	{
		static constexpr ValueType callableType = ((bool(std::invocable<Callable, const Ts&>) * TypeGroup<Ts...>::template IndexOfType<Ts>) + ...);
		using T = TypeGroup<Ts...>::template TypeAtIndex<callableType>;

		if (type != callableType)
			return false;

		if constexpr (SameAs<std::invoke_result_t<Callable, const T&>, bool>)
			return callable(GetValueUnsafe<T>());
		else
		{
			callable(GetValueUnsafe<T>());
			return true;
		}
	}
	template<typename ...Ts>
	template<OneOf<Ts...> T>
	inline constexpr auto Variant<Ts...>::GetValueTypeOf() -> ValueType
	{
		return TypeGroup<Ts...>::template IndexOfType<T>;
	}
	template<typename ...Ts>
	template<typename T>
	inline constexpr bool Variant<Ts...>::IsTypeValid()
	{
		return OneOf<T, Ts...>;
	}
	template<typename ...Ts>
	template<typename ...Ts2>
	inline bool Variant<Ts...>::TryConvert(Variant<Ts2...>& other)
	{
		auto TryMove = [&]<typename T, uintMem index>() {
			if constexpr (OneOf<T, Ts...>)
				if (type == GetValueTypeOf<T>())
				{
					other = std::move(GetValueUnsafe<T>());
					*this = { };

					return false;
				}
			return true;
		};

		bool failedToMove = TypeGroup<Ts2...>::ForEach(TryMove);

		if (failedToMove)
			return false;
		else
			return true;
	}
	template<typename ...Ts>
	inline auto Variant<Ts...>::operator=(const Variant& other) -> Variant& requires (IsConstructibleFrom<Ts, const Ts&> && ...)
	{
		if (type >= sizeof...(Ts) && other.type >= sizeof...(Ts))
			return *this;

		if constexpr ((IsAssignableFrom<Ts, const Ts&> && ...))
			if (type == other.type && other.type < sizeof...(Ts))
			{
				void(*functions[])(Variant&, const Variant&) = { &CopyAssignSpecific<Ts>... };
				Apply<Variant&, const Variant&>(type, functions, *this, other);
				return *this;
			}

		Destruct();
		type = other.type;

		if (other.type >= sizeof...(Ts))
			return *this;

		void(*functions[])(Variant&, const Variant&) = { &CopyConstructSpecific<Ts>... };
		Apply<Variant&, const Variant&>(type, functions, *this, other);
		return *this;
	}
	template<typename ...Ts>
	inline auto Variant<Ts...>::operator=(Variant&& other) noexcept -> Variant& requires (IsConstructibleFrom<Ts, Ts&&> && ...)
	{
		if (type >= sizeof...(Ts) && other.type >= sizeof...(Ts))
			return *this;

		if constexpr ((IsAssignableFrom<Ts, const Ts&> && ...))
			if (type == other.type && other.type < sizeof...(Ts))
			{
				void(*functions[])(Variant&, Variant&&) = { &MoveAssignSpecific<Ts>... };
				Apply<Variant&, Variant&&>(type, functions, *this, std::move(other));
				return *this;
			}

		Destruct();
		type = other.type;

		if (other.type >= sizeof...(Ts))
			return *this;

		void(*functions[])(Variant&, Variant&&) = { &MoveConstructSpecific<Ts>... };
		Apply<Variant&, Variant&&>(type, functions, *this, std::move(other));
		return *this;
	}
	template<typename ...Ts>
	template<OneOf<Ts...> T>
	inline auto Variant<Ts...>::operator=(const T& value) -> Variant& requires IsConstructibleFrom<T, const T&>
	{
		auto newType = GetValueTypeOf<T>();
		if (type != newType)
		{
			Destruct();
			type = newType;

			auto& dst = GetValueUnsafe<T>();
			new (&dst) T(value);
		}
		else
		{
			if constexpr (IsAssignableFrom<T, const T&>)
				GetValueUnsafe<T>() = value;
			else
			{
				Destruct();
				auto& dst = GetValueUnsafe<T>();
				new (&dst) T(value);
			}
		}
		return *this;
	}
	template<typename ...Ts>
	template<OneOf<Ts...> T>
	inline auto Variant<Ts...>::operator=(T&& value) noexcept -> Variant& requires IsConstructibleFrom<T, T&&>
	{
		auto newType = GetValueTypeOf<T>();
		if (type != newType)
		{
			Destruct();
			type = newType;

			auto& dst = GetValueUnsafe<T>();
			new (&dst) T(std::move(value));
		}
		else
		{
			if constexpr (IsAssignableFrom<T, const T&>)
				GetValueUnsafe<T>() = std::move(value);
			else
			{
				Destruct();
				auto& dst = GetValueUnsafe<T>();
				new (&dst) T(std::move(value));
			}
		}
		return *this;
	}
	template<typename ...Ts>
	template<OneOf<Ts...> T>
	inline constexpr T& Variant<Ts...>::GetValueUnsafe()
	{
		return *std::launder(reinterpret_cast<T*>(buffer));
	}
	template<typename ...Ts>
	template<OneOf<Ts...> T>
	inline constexpr const T& Variant<Ts...>::GetValueUnsafe() const
	{
		return *std::launder(reinterpret_cast<const T*>(buffer));
	}
	template<typename ...Ts>
	inline auto Variant<Ts...>::GetValueType() const -> ValueType 
	{
		return type; 
	}
	template<typename ...Ts>
	template<OneOf<Ts...> T>
	inline bool Blaze::Variant<Ts...>::IsType() const
	{
		return type == GetValueTypeOf<T>();
	}
	template<typename ...Ts>
	inline constexpr void Variant<Ts...>::Destruct()
	{		
		using FunctionType = void(*)(Variant&);
		FunctionType functions[] = {&DestructSpecific<Ts>...};

		Apply<Variant&>(type, functions, *this);
	}
	template<typename ...Ts>
	template<OneOf<Ts...> T>
	inline constexpr void Variant<Ts...>::CopyConstructSpecific(Variant& object, const Variant& other)
	{
		new (&object.GetValueUnsafe<T>()) T(other.GetValueUnsafe<T>());
	}
	template<typename ...Ts>
	template<OneOf<Ts...> T>
	inline constexpr void Variant<Ts...>::MoveConstructSpecific(Variant& object, Variant&& other)
	{
		new (&object.GetValueUnsafe<T>()) T(std::move(other.GetValueUnsafe<T>()));
	}
	template<typename ...Ts>
	template<OneOf<Ts...> T>
	inline constexpr void Variant<Ts...>::DestructSpecific(Variant& object)
	{
		object.GetValueUnsafe<T>().~T();
	}
	template<typename ...Ts>
	template<OneOf<Ts...> T>
	inline constexpr void Variant<Ts...>::CopyAssignSpecific(Variant& object, const Variant& other)
	{
		object.GetValueUnsafe<T>() = other.GetValueUnsafe<T>();
	}
	template<typename ...Ts>
	template<OneOf<Ts...> T>
	inline constexpr void Variant<Ts...>::MoveAssignSpecific(Variant& object, Variant&& other)
	{
		object.GetValueUnsafe<T>() = std::move(other.GetValueUnsafe<T>());
	}
	template<typename ...Ts>
	template<typename ...Args>
	inline constexpr void Variant<Ts...>::Apply(ValueType type, void(*functions[])(Args&&...), Args&& ...args)
	{
		if (type >= sizeof...(Ts))
			return;

		functions[type](std::forward<Args>(args)...);
	}
}