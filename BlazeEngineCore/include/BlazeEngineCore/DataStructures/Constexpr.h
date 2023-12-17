#pragma once
#include "BlazeEngineCore/DataStructures/StringView.h"

namespace Blaze::Constexpr
{
	template <typename T, std::size_t N1, std::size_t N2>
	constexpr std::array<T, N1 + N2> ConcatArray(std::array<T, N1> lhs, std::array<T, N2> rhs)
	{
		std::array<T, N1 + N2> result{};
		std::size_t index = 0;

		for (auto& el : lhs) {
			result[index] = std::move(el);
			++index;
		}
		for (auto& el : rhs) {
			result[index] = std::move(el);
			++index;
		}

		return result;
	}

	template<unsigned N>
	struct FixedString {
		char buf[N + 1]{};
		constexpr FixedString(char const* s) {
			for (unsigned i = 0; i != N; ++i) buf[i] = s[i];
		}
		constexpr operator char const* () const { return buf; }
		constexpr operator StringView () const { return StringView(buf, N); }
	};

	template<unsigned N>
	FixedString(char const (&)[N]) -> FixedString<N - 1>;
}