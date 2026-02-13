#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/String/StringView.h"

namespace Blaze
{
	template<typename Char, uintMem N>
	class FixedString
	{
	public:
		Char buffer[N + 1]{};

		constexpr FixedString() = default;
		constexpr FixedString(const FixedString& other) = default;
		template<TriviallyConvertibleCharacter<Char> Char2>
		constexpr FixedString(const Char2(&arr)[N + 1]);

		constexpr uintMem Count() const { return N; }
		constexpr bool Empty() const { return N == 1; };

		constexpr operator const Char* () const { return buffer; }
		constexpr operator GenericStringView<Char> () const { return buffer; }

		constexpr const Char& operator[](uintMem i) const { return buffer[i]; }
	};

	template<typename Char, uintMem N>
	FixedString(const Char(&)[N]) -> FixedString<Char, N - 1>;

	template<typename Char, uintMem N>
	template<TriviallyConvertibleCharacter<Char> Char2>
	inline constexpr FixedString<Char, N>::FixedString(const Char2(&arr)[N + 1])
	{
		for (uintMem i = 0; i < N + 1; ++i)
			buffer[i] = arr[i];
	}
}